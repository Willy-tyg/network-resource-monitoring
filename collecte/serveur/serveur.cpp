#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <set>
#include <sstream>

#include <cctype> // Pour isprint
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/prepared_statement.h> 
#include <cppconn/statement.h> 
#include <mysql_connection.h> 
#include <mysql_driver.h> 

using namespace std;

std::mutex file_mutex;

// Function to extract substrings up to the comma
std::string extractNext(std::string& str) {
    size_t pos = str.find(',');
    std::string token = str.substr(0, pos);
    str = (pos == std::string::npos) ? "" : str.substr(pos + 1);
    return token;
}


bool isPrintableText(const char* buffer, ssize_t length) {
    for (ssize_t i = 0; i < length; ++i) {
        if (!isprint(buffer[i]) && !isspace(buffer[i])) {
            return false;
        }
    }
    return true;
}


bool checkCommand(const char* buffer, ssize_t length) {
    if (length >= 4 && strncmp(buffer, "STOR", 4) == 0) {
        return true;
    }
    if (length >= 3 && strncmp(buffer, "END", 3) == 0) {
        return true;
    }
    return false;
}


void addIPAddressToReceivedData(const std::string& data, std::ofstream& file, const std::string& ipAddress) {
    std::istringstream iss(data);
    std::string line;
    while (std::getline(iss, line)) {
        // Compter le nombre de virgules
        int commaCount = std::count(line.begin(), line.end(), ',');

        // Si la ligne a exactement 8 virgules, on l'ajoute au fichier
        if (commaCount == 11) {
            file << line << "," << ipAddress << std::endl;
        }
    }
}

void handleClient(int* client_socket, const std::string& output_filename) {
    int clientSocket = *client_socket; // Déréférencement du pointeur
    delete client_socket; // Libération de la mémoire après utilisation

   
    // Récupérer l'adresse IP du client
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    if (getpeername(clientSocket, (struct sockaddr*)&clientAddress, &clientAddressLength) == -1) {
        std::cerr << "Erreur lors de la récupération de l'adresse IP du client." << std::endl;
        close(clientSocket);
        return;
    }
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::string clientIPStr = clientIP;

    while (true) {
        char buffer[2048] = {0};
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break; // Déconnexion du client ou erreur de réception
        }

        std::string data(buffer, bytesReceived);
        // Vérifier si les données commencent par "INFO:"
        const std::string infoPrefix = "INFO:";
        if (data.compare(0, infoPrefix.length(), infoPrefix) == 0) {
            // Séparer "INFO:" du reste des données
            data = data.substr(infoPrefix.length());

            std::cout << "J'ai reçu des données INFO: " << data << std::endl;

            // Variables pour stocker les données extraites
            std::string timestamp, hostname, mac_address, system, system_version, architecture;
            int cpu_cores;
            std::int64_t ram_total, disk_total, swap_total;

            try {
                // Extraire les valeurs
                timestamp = extractNext(data);
                cpu_cores = std::stoi(extractNext(data));
                ram_total = std::stoll(extractNext(data));
                disk_total = std::stoll(extractNext(data));
                hostname = extractNext(data);
                mac_address = extractNext(data);
                swap_total = std::stoll(extractNext(data));
                system = extractNext(data);
                system_version = extractNext(data);
                architecture = extractNext(data);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
                close(clientSocket);
                return;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range error: " << e.what() << std::endl;
                close(clientSocket);
                return;
            }

            std::string databaseIP = "127.0.0.1";

            try {
                // Connect to MySQL database
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                std::string link = "tcp://" + databaseIP + ":3306";
                std::unique_ptr<sql::Connection> con(driver->connect(link, "root", ""));
                con->setSchema("node");

                // Create a prepared statement for inserting log entries
                std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
                    "INSERT INTO SystemInfo(timestamp, cpu_cores, ram_total, disk_total, hostname, mac_address, swap_total, system, system_version, architecture) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                ));

                pstmt->setString(1, timestamp);
                pstmt->setInt(2, cpu_cores);
                pstmt->setUInt64(3, ram_total);
                pstmt->setUInt64(4, disk_total);
                pstmt->setString(5, hostname);
                pstmt->setString(6, mac_address);
                pstmt->setUInt64(7, swap_total);
                pstmt->setString(8, system);
                pstmt->setString(9, system_version);
                pstmt->setString(10, architecture);

                pstmt->executeUpdate();
            } catch (sql::SQLException &e) {
                std::cerr << "SQL error: " << e.what() << std::endl;
                close(clientSocket);
                return;
            }
        }
        else {
            std::string command(buffer, bytesReceived);
            if (command.substr(0, 4) == "STOR") {
                file_mutex.lock();
                std::ofstream file;
                {
                    file.open(output_filename, std::ios_base::app);
                    if (!file.is_open()) {
                        std::cerr << "Unable to open file: " << output_filename << std::endl;
                        close(clientSocket);
                        return;
                    }
                }

                // Boucle de réception des données
                while (true) {
                    ssize_t bytes_read = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytes_read <= 0) {
                        break;
                    }
                    std::string data(buffer, bytes_read);

                    if (data.find("END") != std::string::npos) {
                        break;
                    }

                    if (data.find("STOR") == std::string::npos) {
                        
                        addIPAddressToReceivedData(data, file, clientIPStr);
                        

                    }
                }

                file.close();
                file_mutex.unlock();
                std::cout << "Received data written to file: " << output_filename << std::endl;
            }
        }
    }

    close(clientSocket);
}


void* insertData(void* args) {
    auto* params = static_cast<std::pair<std::string, std::string>*>(args);
    std::string filename = params->first;
    std::string databaseIP = params->second;
    delete params;  // Free the allocated memory for params

    while (true) {
        std::ifstream datafile(filename);
        if (!datafile.is_open()) {
            std::cerr << "Failed to open the file" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before retrying
            continue;
        }

        try {
            // Connect to MySQL database
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::string link = "tcp://" + databaseIP + ":3306";
            std::unique_ptr<sql::Connection> con(driver->connect(link, "root", ""));
            con->setSchema("node");

            // Ouverture du fichier en lecture
            file_mutex.lock();
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
                continue;
            }

            std::string line;
            while (std::getline(file, line)) {
                // Variables pour stocker les données extraites
                std::string timestamp, ram_free, disk_unused, ip_address, cpu_frequency, macAddress, uptime, last_boot, cache_size, swap_free, active_process, cpu_temperature, battery;

                // Copie de la ligne dans une variable modifiable
                std::string data = line;

                // Trouver les positions des virgules pour extraire les données
                timestamp = extractNext(data);
                ram_free = extractNext(data);
                disk_unused = extractNext(data);
                cpu_frequency = extractNext(data);
                macAddress = extractNext(data);
                uptime = extractNext(data);
                last_boot = extractNext(data);
                cache_size = extractNext(data);
                swap_free = extractNext(data);
                active_process = extractNext(data);
                cpu_temperature = extractNext(data);
                battery = extractNext(data);
                ip_address = extractNext(data);
                auto now = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                std::string connection_time = std::ctime(&now_time);
                // Create a prepared statement for inserting log entries
                std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
                    "INSERT INTO SystemInfoVariable(timestamp, ram_free, disk_unused, cpu_frequency, mac_address, uptime, last_boot, cache_size, swap_free, active_process, cpu_temperature,battery, connection_time, ip_address) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                ));

                pstmt->setString(1, timestamp);
                pstmt->setString(2, ram_free);
                pstmt->setString(3, disk_unused);
                pstmt->setString(4, cpu_frequency);
                pstmt->setString(5, macAddress);
                pstmt->setString(6, uptime);
                pstmt->setString(7, last_boot);
                pstmt->setString(8, cache_size);
                pstmt->setString(9, swap_free);
                pstmt->setString(10, active_process);
                pstmt->setString(11, cpu_temperature);
                pstmt->setString(12, battery);
                pstmt->setString(13, connection_time);
                pstmt->setString(14,ip_address);

                pstmt->executeUpdate();
            }

            file.close();
            file_mutex.unlock();

            // Clear the file after processing
            std::ofstream ofs(filename, std::ios::trunc);
            ofs.close();

            datafile.close();
        } catch (sql::SQLException &e) {
            std::cerr << "SQLException: " << e.what() << std::endl;
            std::cerr << "SQLState: " << e.getSQLState() << std::endl;
            std::cerr << "Error Code: " << e.getErrorCode() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before the next cycle
    }

    return nullptr;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Échec de la création du socket." << std::endl;
        return 1;
    }

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("Erreur lors du paramétrage de l'option SO_REUSEADDR");
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    int portno = 12345;
    serverAddress.sin_port = htons(portno);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Échec de la liaison du socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 50) == -1) {
        std::cerr << "Échec de l'écoute sur le socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    const std::string received_filename = "received_system_info.csv";
    std::cout << "Serveur en écoute sur le port : " << portno << std::endl;

    auto* params = new std::pair<std::string, std::string>(received_filename, "127.0.0.1");
    pthread_t insert_thread;
    pthread_create(&insert_thread, nullptr, insertData, params);
    pthread_detach(insert_thread);

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int* clientSocket = new int(accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength));
        if (*clientSocket == -1) {
            std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
            delete clientSocket;
            continue;
        }

        std::cout << "Client connecté: " << inet_ntoa(clientAddress.sin_addr) << std::endl;
    
        // Démarrer un thread pour gérer le client
        std::thread(handleClient, clientSocket, std::ref(received_filename)).detach();
    }

    close(serverSocket);
    return 0;
}
