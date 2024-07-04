#!/bin/bash
SERVICE_NAME="system-info-collector"
EXECUTABLE_NAME="client"
EXECUTABLE_PATH="/usr/local/bin/$SERVICE_NAME"
SERVICE_FILE_PATH="/etc/systemd/system/$SERVICE_NAME.service"

DATA_DIRECTORY="/var/lib/$SERVICE_NAME"

# Vérifier si les fichiers existent déjà
if [ -f "$SERVICE_FILE_PATH" ] || [ -f "$DATA_DIRECTORY/system_info_var.csv" ] || [ -f "$DATA_DIRECTORY/temp_file.csv" ]; then
  echo "Erreur : Les fichiers ou le service existent déjà."
  exit 1
fi

# Créer les répertoires de journaux et de données s'ils n'existent pas
mkdir -p "$DATA_DIRECTORY"

# Définir les noms de fichiers
filename="$DATA_DIRECTORY/system_info_var.csv"
tempFileName="$DATA_DIRECTORY/temp_file.csv"

# Créer les fichiers avec des permissions adéquates s'ils n'existent pas
touch "$filename"
chmod 777 "$filename"

touch "$tempFileName"
chmod 777 "$tempFileName"

# Définir les propriétaires des fichiers
chown nobody:nogroup "$filename" "$tempFileName"

# Vérifier si l'utilisateur est root
if [ "$EUID" -ne 0 ]; then
  echo "Veuillez exécuter ce script en tant que root."
  exit 1
fi

# Arrêter le service existant s'il est en cours d'exécution
systemctl stop "$SERVICE_NAME.service" || true

# Copier l'exécutable vers le chemin spécifié
if [ ! -f "./$EXECUTABLE_NAME" ]; then
  echo "Erreur : le fichier exécutable $EXECUTABLE_NAME n'a pas été trouvé dans le répertoire courant."
  exit 1
fi

cp "./$EXECUTABLE_NAME" "$EXECUTABLE_PATH"
chmod +x "$EXECUTABLE_PATH"

# Définir les propriétaires des répertoires de journaux et de données
chown -R nobody:nogroup "$DATA_DIRECTORY"

# Créer le fichier de service systemd avec redémarrage périodique
cat <<EOL > "$SERVICE_FILE_PATH"
[Unit]
Description=Client Service
After=network.target

[Service]
ExecStart=$EXECUTABLE_PATH 127.0.0.1
Restart=always
User=nobody
Group=nogroup
Environment="DATA_DIRECTORY=$DATA_DIRECTORY"
StandardOutput=null
StandardError=null

[Install]
WantedBy=multi-user.target
EOL

# Recharger systemd pour inclure le nouveau service
systemctl daemon-reload

# Activer et démarrer le service
systemctl enable "$SERVICE_NAME.service"
systemctl start "$SERVICE_NAME.service"

# Vérifier le statut du service
systemctl status "$SERVICE_NAME.service"

echo "Le service $SERVICE_NAME a été installé et démarré avec succès."
echo "Pour vérifier les journaux du service, utilisez la commande suivante :"
echo "sudo journalctl -u $SERVICE_NAME.service"

# Instructions pour copier et exécuter ce script sur d'autres machines
echo "Pour installer ce service sur d'autres machines, copiez ce script et le fichier exécutable $EXECUTABLE_NAME sur les autres machines et exécutez le script en tant que root."
