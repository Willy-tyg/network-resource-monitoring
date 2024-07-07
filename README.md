Application de Collecte et de Visualisation des Ressources

Cette application permet de collecter et de visualiser l'utilisation des ressources des machines sur un réseau. Elle est composée de deux parties : un client écrit en C++ qui collecte les données sur chaque machine et un serveur qui reçoit ces données et les insère dans une base de données. La visualisation des données est faite à l'aide de PHP, HTML, CSS et JavaScript.



*Clonage du Dépôt

Commencez par cloner le dépôt GitHub sur votre machine locale.

	https://github.com/Willy-tyg/network-resource-monitoring.git
	cd network-resource-monitoring
	
*Installation des Dépendances

	sudo apt-get install apache2 php libapache2-mod-php php-mysql mysql-server
	sudo apt-get install software-properties-common
	sudo apt-add-repository 'deb http://repo.mysql.com/apt/ubuntu/ focal mysql-8.0'
	sudo apt-get update
	
	sudo apt-get install build-essential libmysqlclient-dev
	sudo apt-get install libmysqlcppconn-dev


*Configurer la base de données  :	

	sudo mysql -u root -p
	# Puis, dans l'interface MySQL :
	CREATE DATABASE node;
	USE node;
	SOURCE visualisation/database/node.sql;


*Lancer le serveur et lagent
	*Serveur	
	    Compiler le code serveur :
		cd collecte/serveur
		make

	*Client ou agent de collecte

	    *Installer les bibliothèques nécessaires pour le client C++ :

		sudo apt-get update
		sudo apt-get install build-essential

	    *Compiler le code client :

		cd collecte/client
		make
		
	    *Donner les droits d'éxécution au fichier 'client' et au script 'script.sh' présent dans le dossier collecte/client/
	    
	    	cd collecte/client
	    	sudo chmod +x *
	    	
	    *Specifier l'adresse IP du serveur dans le fichier 'script.sh'
	   
	   	cd collecte/client
	    	sudo nano script.sh
	    	remplacer '127.0.0.1' a la ligne 60 par l'adresse du serveur 
	    	
	    *Lancer le script pour démarrer le service
	    
	    	cd collecte/client
	    	sudo ./script.sh

*Application Web



	*Lancement de l'Application

	    *Démarrer Apache :
		sudo systemctl start apache2
		
	*mettez à jour vos informations de connexion a la base de données
	    *Dans le fichier initialize.php du dossier visualisation mettez à jour les infos 
	    	ligne6: DB_SERVER: adresse ip du serveur php
		ligne7: DB_USERNAME: utilisateur de la bd;
		ligne8: DB_PASSWORD: mot de passe de l'user de la bd;
		ligne9: DB_NAME: nom de la base de donnée(node);

    	*Déployer l'application web :

	    *Copiez le dossier "visualisation" dans le répertoire web de votre serveur Apache.

		sudo cp -r visualisation /var/www/html/

*Visualisation des Données

	*Ouvrez un navigateur web et accédez à l'adresse IP de votre serveur pour voir l'interface de visualisation des données.

	        http://[Adresse IP de votre serveur]
	*une interface de connexion apparaitra utilisez ces identifiants:
		username: admin
		pass: admin123
		
		
		
		
		
		
		
		
SI VOUS AVEZ UN AUTRE SERVEUR WEB, vous pouvez juste deposez le dossier à la bonne emplacement et bien configurer les chemins d'accès.

NOS CONTACTS: gireswilly32@gmail.com / +237680417185
