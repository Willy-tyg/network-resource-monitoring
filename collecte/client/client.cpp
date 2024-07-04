#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sstream>
#include <mutex>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <stdexcept>
#include <array>
#include <regex>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <array>
#include <cstdio>
#include <memory>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;
std::mutex file_mutex;


struct SystemInfo {
    std::string timestamp;
    int cpu_cores;
    long ram_total;
    long disk_total;
    std::string hostname;
    std::string mac_address;
    long swap_total;
    std::string system;
    std::string system_version;
    std::string architecture;
};


struct SystemInfoVariable {
    std::string timestamp;
    long ram_free;
    long disk_unused;
    long cpu_frequency;
    std::string mac_address;
    string uptime;
    string last_boot;
    long cache_size;
    long swap_free;
    int active_process;
    long cpu_temperature;
    string battery;
    
};


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


std::string getMACAddress() {
    std::array<char, 128> buffer;
    std::string result;
    std::string command = "cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        std::cerr << "Erreur lors de l'exécution de la commande pour l'adresse MAC" << std::endl;
        return "00:00:00:00:00:00"; // Valeur par défaut en cas d'erreur
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(result.find_last_not_of(" \n\r\t") + 1); // Supprimer les espaces blancs de fin
    return result;
}


SystemInfo collectSystemInfo() {
    SystemInfo info;

    // Récupérer le temps actuel comme horodatage
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    info.timestamp = std::ctime(&now_c);
    info.timestamp.pop_back(); // Supprimer le saut de ligne final

    // Récupérer les informations CPU: nombre de coeurs
    std::ifstream cpuinfo("/proc/cpuinfo");
    int coreCount = 0;
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("processor") != std::string::npos) {
            coreCount++;
        }
    }
    cpuinfo.close();
    info.cpu_cores = coreCount;

    // Récupérer les informations sur la mémoire swap total et ram total
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier /proc/meminfo" << std::endl;
    }
    
    unsigned long long total_memory = 0, total_swap = 0;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            total_memory = std::stoll(line.substr(line.find_first_of(" ") + 1));
        } else if (line.find("SwapTotal:") != std::string::npos) {
            total_swap = std::stoll(line.substr(line.find_first_of(" ") + 1));
        }
    }
    meminfo.close();
    info.ram_total = total_memory;
    info.swap_total = total_swap;

    // Récupérer le nom d'hôte
    char hostname[1024];
    gethostname(hostname, 1024);
    info.hostname = hostname;

 

    // Récupérer l'adresse MAC
     // Récupérer l'adresse MAC
    info.mac_address = getMACAddress();


    // Récupérer l'utilisation du disque
    struct statvfs stat;
    statvfs("/", &stat);
    unsigned long long total_space = static_cast<unsigned long long>(stat.f_blocks) * stat.f_frsize;
    info.disk_total = total_space;

    // Récupérer les informations du système
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        info.system = unameData.sysname;
        info.system_version = unameData.release;
        info.architecture = unameData.machine;
    } else {
        std::cerr << "Erreur lors de la récupération des informations système." << std::endl;
    }

    return info;
}



SystemInfoVariable collectSystemInfoVar() {
    SystemInfoVariable info;

    // Récupérer le temps actuel comme horodatage
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    info.timestamp = std::ctime(&now_c);
    info.timestamp.pop_back(); // Supprimer le saut de ligne final

    // Récupérer les informations CPU
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("cpu MHz") != std::string::npos) {
            info.cpu_frequency = std::stol(line.substr(line.find(":") + 2));
        }
    }
    cpuinfo.close();


    // Récupérer les informations sur la mémoire
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    info.ram_free = memInfo.freeram * memInfo.mem_unit;
    info.swap_free = memInfo.freeswap * memInfo.mem_unit;




    // Récupérer l'adresse MAC
    info.mac_address = getMACAddress();



   // Récupérer l'uptime
    std::ifstream uptimeFile("/proc/uptime");
    if (uptimeFile.is_open()) {
        double uptimeSeconds;
        uptimeFile >> uptimeSeconds;
        info.uptime = std::to_string(uptimeSeconds);
        
        uptimeFile.close();
    } else {
        std::cerr << "Error opening /proc/uptime" << std::endl;
    }

    // Récupération de la dernière heure de démarrage
    std::ifstream uptime_file("/proc/uptime");
    if (uptime_file.is_open()) {
        double uptime_seconds;
        uptime_file >> uptime_seconds;
        uptime_file.close();

        time_t boot_time = time(nullptr) - static_cast<time_t>(uptime_seconds);
        info.last_boot = ctime(&boot_time);
    } else {
        info.last_boot = "UNKNOW";
    }

    // Récupérer l'espacr dsque non utilisé
    struct statvfs stat;
    statvfs("/", &stat);
    unsigned long long available_space = static_cast<unsigned long long>(stat.f_bavail) * stat.f_frsize;
    info.disk_unused = available_space;

    // Récupérer la taille du cache 
    info.cache_size = memInfo.bufferram * memInfo.mem_unit;

    //recuperer le nombre de processus actif
    std::vector<pid_t> processIDs;
    DIR* dir = opendir("/proc");
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (std::all_of(entry->d_name, entry->d_name + std::strlen(entry->d_name), [](char c) { return std::isdigit(c); })) {
                processIDs.push_back(std::stoi(entry->d_name));
            }
        }
        closedir(dir);
    }
    info.active_process =  processIDs.size();


    //recuperer la temperature
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (tempFile.is_open()) {
        std::string temp;
        std::getline(tempFile, temp);
        try {
            int tempValue = std::stoi(temp) / 1000; // Convertir de millidegrés Celsius en degrés Celsius
            info.cpu_temperature = tempValue;
        } catch (const std::invalid_argument& e) {
            //std::cerr << "Erreur de conversion de la température : " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            // std::cerr << "Dépassement de plage lors de la conversion : " << e.what() << std::endl;
        }

        tempFile.close();
    } else {
        std::cerr << "Erreur d'ouverture du fichier de température." << std::endl;
    }


    //récupérer le pourcentage de la batterie
    std::string batteryPath = "/sys/class/power_supply/BAT0/capacity";
    std::ifstream file(batteryPath);

    if (!file) {
        std::cerr << "Unable to open file: " << batteryPath << std::endl;
        
    }

    std::string capacity;
    std::getline(file, capacity);
    file.close();

    if (!capacity.empty()) {
        info.battery =  capacity;
    } else {
        std::cerr << "Failed to read battery capacity." << std::endl;
    }


    return info;
}

std::string toCSV(const SystemInfo& info) {
    std::ostringstream oss;
    oss << info.timestamp << ","
        << info.cpu_cores << ","
        << info.ram_total << ","
        << info.disk_total << ","
        << info.hostname << ","
        << info.mac_address << ","
        << info.swap_total << ","
        << info.system << ","
        << info.system_version << ","
        << info.architecture;
    return oss.str();
}


std::string sanitizeCSVField(const std::string& field) {
    std::string sanitizedField = field;
    sanitizedField.erase(std::remove(sanitizedField.begin(), sanitizedField.end(), '\n'), sanitizedField.end());
    sanitizedField.erase(std::remove(sanitizedField.begin(), sanitizedField.end(), '\r'), sanitizedField.end());
    return sanitizedField;
}

std::string toCSVvar(const SystemInfoVariable& info) {
    std::ostringstream oss;
    oss << sanitizeCSVField(info.timestamp) << ","
        << info.ram_free << ","
        << info.disk_unused << ","
        << info.cpu_frequency << ","
        << info.mac_address << ","
        << info.uptime << ","
        << sanitizeCSVField(info.last_boot) << ","
        << info.cache_size << ","
        << info.swap_free << ","
        << info.active_process << ","
        << info.cpu_temperature << ","
        << info.battery;

    return oss.str();
}



void writeCSVToFile(const std::string& csv, const std::string& filename) {
    std::lock_guard<std::mutex> guard(file_mutex);
    std::ofstream file(filename, std::ios_base::app); // mode append
    if (file.is_open()) {
        file << csv << std::endl;
        file.close();
        std::cout << "Données écrites dans le fichier : " << filename << std::endl;
    } else {
        std::cerr << "Impossible d'ouvrir le fichier : " << filename << std::endl;
    }
}



int sendFileOverSocket(const std::string& csv, int clientSocket) {
    const char* request = "STOR";
    send(clientSocket, request, strlen(request) + 1, 0);

    file_mutex.lock();
    std::ifstream file(csv);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier à envoyer : " << csv << std::endl;
        return -1;
    }

    std::string line;
    std::vector<std::string> linesBuffer;
    const int linesPerBlock = 20;
    while (std::getline(file, line)) {
        linesBuffer.push_back(line);

        if (linesBuffer.size() >= linesPerBlock) {
            std::ostringstream oss;
            for (const auto& l : linesBuffer) {
                oss << l << "\n";
            }
            std::string block = oss.str();

            ssize_t bytesSent = send(clientSocket, block.c_str(), block.size(), 0);
            if (bytesSent == -1) {
                std::cerr << "Erreur : Échec de l'envoi des données du fichier." << std::endl;
                file.close();
                return -1;
            }
            linesBuffer.clear();
        }
    }

    // Send any remaining lines
    if (!linesBuffer.empty()) {
        std::ostringstream oss;
        for (const auto& l : linesBuffer) {
            oss << l << "\n";
        }
        std::string block = oss.str();

        ssize_t bytesSent = send(clientSocket, block.c_str(), block.size(), 0);
        if (bytesSent == -1) {
            std::cerr << "Erreur : Échec de l'envoi des données du fichier." << std::endl;
            file.close();
            return -1;
        }
    }


    file.close();
    file_mutex.unlock();

    const char* end_request = "END";
    send(clientSocket, end_request, strlen(end_request) + 1, 0);

    std::cout << "Données envoyées avec succès au serveur." << std::endl;
    return 0;
}


int sendInfoOverSocket(const std::string& data, int clientSocket) {

    char buffer[1024];
    string csv = "INFO:"+data;
    if (send(clientSocket, csv.c_str(), csv.size(), 0) == -1) {
        std::cerr << "ERROR: Données statiques non envoyées!" << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Info statiques envoyées avec succès au serveur." << std::endl;
    return 0;
}

bool checkFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        // Le fichier n'existe pas, on tente de le créer
        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Erreur : Impossible de créer le fichier " << filename << std::endl;
            return false;
        }
        outfile.close();
        std::cout << "Fichier " << filename << " créé avec succès." << std::endl;
        return true; // Fichier créé avec succès
    }

    // Le fichier existe, vérifier s'il est vide
    return file.tellg() > 0;
}


void appendFile(const std::string& sourcePath, const std::string& destinationPath) {
    file_mutex.lock();
    std::ifstream src(sourcePath, std::ios::binary);
    std::ofstream dst(destinationPath, std::ios::binary | std::ios::app);

    if (!src.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier des infos." << std::endl;
        return;
    }

    if (!dst.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier temporaire." << std::endl;
        return;
    }

    dst << src.rdbuf();

    std::cout << "Les infos ont été enregstrés dans le fichier temporaire" << std::endl;
    file_mutex.unlock();
}




const int SERVER_PORT = 12345;
const int COLLECTION_INTERVAL = 2;  // secondes
const int SOCKET_SEND_INTERVAL = 6;  // secondes


int main(int argc, char *argv[]) {
    std::string SERVER_HOST;

    if (argc > 1) {
        // Si un argument est passé, utilisez-le comme adresse IP du serveur
        SERVER_HOST = argv[1];
    } else {
        // Sinon, utilisez l'adresse IP par défaut
        SERVER_HOST = "192.168.168.174";
    }



  
    const std::string filename = "/var/lib/system-info-collector/system_info_var.csv";

    std::string tempFileName = "/var/lib/system-info-collector/temp_file.csv";
    
    bool send_first = false;


    SystemInfoVariable info = collectSystemInfoVar();
    std::string csvVar = toCSVvar(info);
    writeCSVToFile(csvVar, filename);
   
    auto last_collect_time = std::chrono::steady_clock::now();
    auto last_send_time = std::chrono::steady_clock::now();

    try {
        while (true) {
            auto current_time = std::chrono::steady_clock::now();

            // Collect system info at regular intervals
            auto elapsed_time_collect = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_collect_time).count();
            if (elapsed_time_collect >= COLLECTION_INTERVAL) {
                SystemInfoVariable info = collectSystemInfoVar();
                std::string csvVar = toCSVvar(info);
                writeCSVToFile(csvVar, filename);
                last_collect_time = std::chrono::steady_clock::now();
            }

            // Send data over socket at regular intervals
            auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_send_time).count();
            
            if (elapsed_time >= SOCKET_SEND_INTERVAL) {
                int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
                if (clientSocket == -1) {
                    std::cerr << "Failed to create socket." << std::endl;
                    return 1;
                }

                struct sockaddr_in serverAddress;
                serverAddress.sin_family = AF_INET;
                serverAddress.sin_port = htons(SERVER_PORT);
                serverAddress.sin_addr.s_addr = inet_addr(SERVER_HOST.c_str());

                // Attempt to connect to the server
                if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
                    std::cerr << "Error connecting to server." << std::endl;
                    close(clientSocket);

                    // Save data to temporary file on connection failure
                    appendFile(filename, tempFileName);
                    std::ofstream ofs(filename, std::ios::trunc);
                    ofs.close();

                    continue;  // Move to next iteration
                }
                std::cout << "Connected to server" << std::endl;

                if(!send_first){
                    SystemInfo info = collectSystemInfo();
                    std::string infos = toCSV(info);
                    sendInfoOverSocket(infos, clientSocket);
                    send_first = true;
                }

                
                if (checkFile(tempFileName)) {
                    if (sendFileOverSocket(tempFileName, clientSocket) == 0) {
                        last_send_time = std::chrono::steady_clock::now();
                        std::ofstream ofs(tempFileName, std::ios::trunc);
                        ofs.close();
                    }
                } 


                // Send file over socket
                if (sendFileOverSocket(filename, clientSocket) == 0) {
                    last_send_time = std::chrono::steady_clock::now();
                    std::ofstream ofs(filename, std::ios::trunc);
                    ofs.close();
                } else {
                    // Save data to temporary file on send failure
                    appendFile(filename, tempFileName);
                    std::ofstream ofs(filename, std::ios::trunc);
                    ofs.close();
                }

                close(clientSocket);  // Close connection
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}