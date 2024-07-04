<?php
if(isset($_GET['mac_address'])){
    $mac_address = $_GET['mac_address'];
    $qry = $conn->query("SELECT * FROM SystemInfoVariable WHERE mac_address = '$mac_address'");
    $data_points = [];
    if($qry->num_rows > 0){
        while ($row = $qry->fetch_assoc()) {
            $data_points[] = $row;
        }
    }
}
?>

<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Graphiques des Variations</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns"></script>
</head>
<body>

<canvas id="ramChart" width="400" height="200"></canvas>
<canvas id="cpuChart" width="400" height="200"></canvas>
<canvas id="temperatureChart" width="400" height="200"></canvas>

<canvas id="diskChart" width="400" height="200"></canvas>

<script>
    // Récupérer les données de PHP
    const data = <?php echo json_encode($data_points); ?>;
    console.log(data);

    if (data.length > 0) {
        // Extraire les valeurs pour chaque graphique
        const labels = data.map(item => new Date(item['timestamp']));
        const ramUnused = data.map(item => parseFloat(item['ram_free']) / (1024 * 1024 * 1024));
        const cpuFrequency = data.map(item => parseFloat(item['cpu_frequency']));
        const temperature = data.map(item => parseFloat(item['cpu_temperature']));

        const diskUnused = data.map(item => parseFloat(item['disk_unused']) / (1024 * 1024 * 1024));


        // Configurer les graphiques individuels
        const ramCtx = document.getElementById('ramChart').getContext('2d');
        const ramChart = new Chart(ramCtx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: 'Ram inutilisée',
                    data: ramUnused,
                    borderColor: 'rgba(75, 192, 192, 1)',
                    backgroundColor: 'rgba(75, 192, 192, 0.2)',
                    fill: false,
                }]
            },
            options: {
                responsive: true,
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'minute'
                        },
                        title: {
                            display: true,
                            text: 'Heure'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Valeurs'
                        }
                    }
                }
            }
        });

        const cpuCtx = document.getElementById('cpuChart').getContext('2d');
        const cpuChart = new Chart(cpuCtx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: 'Fréquence CPU',
                    data: cpuFrequency,
                    borderColor: 'rgba(54, 162, 235, 1)',
                    backgroundColor: 'rgba(54, 162, 235, 0.2)',
                    fill: false,
                }]
            },
            options: {
                responsive: true,
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'minute'
                        },
                        title: {
                            display: true,
                            text: 'Heure'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Valeurs'
                        }
                    }
                }
            }
        });

        const tempCtx = document.getElementById('temperatureChart').getContext('2d');
        const tempChart = new Chart(tempCtx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: 'Température',
                    data: temperature,
                    borderColor: 'rgba(255, 99, 132, 1)',
                    backgroundColor: 'rgba(255, 99, 132, 0.2)',
                    fill: false,
                }]
            },
            options: {
                responsive: true,
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'minute'
                        },
                        title: {
                            display: true,
                            text: 'Heure'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Valeurs'
                        }
                    }
                }
            }
        });


        const diskCtx = document.getElementById('diskChart').getContext('2d');
        const diskChart = new Chart(diskCtx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: 'DISQUE INUTILISÉE',
                    data: diskUnused,
                    borderColor: 'rgba(54, 162, 235, 1)',
                    backgroundColor: 'rgba(54, 162, 235, 0.2)',
                    fill: false,
                }]
            },
            options: {
                responsive: true,
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'minute'
                        },
                        title: {
                            display: true,
                            text: 'Heure'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Valeurs'
                        }
                    }
                }
            }
        });

    } else {
        console.log("Aucune donnée disponible pour afficher les graphiques.");
    }
</script>

</body>
</html>
