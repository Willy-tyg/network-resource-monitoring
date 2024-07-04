<?php
if(isset($_GET['mac_address'])){
    $qry = $conn->query("SELECT * FROM SystemInfo where mac_address = '{$_GET['mac_address']}'");
    if($qry->num_rows > 0){
        $res = $qry->fetch_array();
        foreach($res as $k => $v){
            if(!is_numeric($k))
            $$k = $v;
        }
    }

    $mac_address = $_GET['mac_address'];

}
?>

<div class="container-fluid">
    <div class="card-header">
        <h3 class="card-title">Caracteristiques de la machine </h3>
    </div>
    <style>
        #sys_logo {
            width: 5em;
            height: 5em;
            object-fit: scale-down;
            object-position: center center;
        }
        /* Styles pour embellir les tableaux */
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px; /* Ajout d'espace entre les tableaux */
        }
        table th, table td {
            padding: 10px;
            white-space: nowrap;
            text-align: left;
            border: 1px solid #ddd;
        }
        table thead {
            background-color: #f8f9fa;
            color: #333;
            border-bottom: 2px solid #ddd;
        }
        table tbody tr:nth-child(even) {
            background-color: #f2f2f2;
        }
        table tbody tr:hover {
            background-color: #e9ecef;
        }
        .card {
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            border-radius: 5px;
            overflow: hidden;
        }
        .card-header {
            background-color: #007bff;
            color: white;
            padding: 10px 15px;
        }
        .card-title {
            margin: 0;
            font-size: 1.25rem;
        }
        .table-spacing {
            margin-top: 20px; /* Modifier la valeur selon vos besoins */
        }
        .table-container {
            overflow-x: auto;
            background: white;
        }
        .scroll-bar {
            background-color: #007bff;
            height: 2px;
            width: 100%;
            position: relative;
            top: 0;
        }
        /* Styles spécifiques pour l'en-tête du premier tableau */
        .first-table th {
            font-size: 14px; /* Taille de police plus petite */
            font-family: Arial, sans-serif; /* Changer la police */
            padding: 8px; /* Réduire l'espacement */
        }
    </style>
    <fieldset>
        <div class="table-container">
            <div class="scroll-bar"></div>
            <table class="table table-bordered table-hover table-striped first-table">
                <thead>
                    <tr class="bg-gradient-dark text-light">
                        <th>ID</th>
                        <th>Hostname</th>
                        <th>Adresse Mac</th>
                        <th>SE</th>
                        <th>Version</th>
                        <th>DD</th>
                        <th>Nb de coeurs</th>
                        <th>Ram</th>
                        <th>Swap</th>
                        <th>Archit</th>
                    </tr>
                </thead>
                <tbody>
                    <?php 
                    $i = 1;  
                    $qry = $conn->query("SELECT * FROM SystemInfo where mac_address = '{$_GET['mac_address']}'");
                    while($row = $qry->fetch_assoc()):
                    ?>
                    <tr>
                        <td><?php echo $row['id'] ?></td>
                        <td><?php echo $row['hostname'] ?></td>
                        <td><?php echo $row['mac_address'] ?></td>
                        <td><?php echo $row['system'] ?></td>
                        <td><?php echo $row['system_version'] ?></td>
                        <td><?php echo $row['disk_total'] ?></td>
                        <td><?php echo $row['cpu_cores'] ?></td>
                        <td><?php echo $row['ram_total'] ?></td>
                        <td><?php echo $row['swap_total'] ?></td>
                        <td><?php echo $row['architecture'] ?></td>
                    </tr>
                    <?php endwhile; ?>
                </tbody>
            </table>
        </div>
    </fieldset>
</div>

<div class="card card-outline card-primary rounded-0 shadow table-spacing">

<div class="card-header" style="background-color: #007bff; color: white;">
    <h3 class="card-title float-left">Caractéristiques variables de la Machine</h3>
    <div class="float-right">
        <a href="./?page=node/view_analyse&mac_address=<?= htmlspecialchars($mac_address) ?>" class="btn btn-sm btn-outline-light"><i class="fas fa-chart-bar"></i> Voir les analyses</a>
        &nbsp; <!-- Espace non rompable -->
        <a href="./?page=node/view_graph&mac_address=<?= htmlspecialchars($mac_address) ?>" class="btn btn-sm btn-outline-light"><i class="fas fa-chart-bar"></i> Voir les graphes</a>
    </div>
    <div class="clearfix"></div>
</div>



    <div class="card-body">
        <div class="container-fluid">
            <div class="table-responsive">
            <div class="container-fluid">
                <table id="machine-list-table" class="table table-bordered table-hover table-striped">
    

                
                <thead>
                    <tr class="bg-gradient-dark text-light">
                        <th>id</th>
                        <th>Heure</th>
                        <th>Ram inutilisée</th>
                        <th>DD disponible</th>
                        <th>Frequence cpu</th>
                        <th>Uptime</th>
                        <th>Dernier démarrage</th>
                        <th>Taille cache</th>
                        <th>Swap unused</th>
                        <th>Nombres de processus</th>
                        <th>temperature</th>
                        <th>% batterie</th>
                        <th>heure d'envoi</th>
                        <th>adresse ip</th>
                    </tr>
                </thead>
                <tbody>
                    <?php 
                    $i = 1;  
                    $qry = $conn->query("SELECT * FROM SystemInfoVariable  where mac_address = '{$_GET['mac_address']}'");
                    while($row = $qry->fetch_assoc()):
                    ?>
                   
                    <tr>
                        <td><?php echo $row['id'] ?></td>
                        <td><?php echo $row['timestamp'] ?></td>
                        <td><?php echo $row['ram_free'] ?></td>
                        <td><?php echo $row['disk_unused'] ?></td>
                        <td><?php echo $row['cpu_frequency'] ?></td>
                        <td><?php echo $row['uptime'] ?></td>
                        <td><?php echo $row['last_boot'] ?></td>
                        <td><?php echo $row['cache_size'] ?></td>
                        <td><?php echo $row['swap_free'] ?></td>
                        <td><?php echo $row['active_process'] ?></td>
                        <td><?php echo $row['cpu_temperature'] ?></td>
                        <td><?php echo $row['battery'] ?></td>
                        <td><?php echo $row['connection_time'] ?></td>
                        <td><?php echo $row['ip_address'] ?></td>
                        
                    </tr>
                    <?php endwhile; ?>
                </tbody>
            </table>
            </div>
            </div>
        </div>
    </div>
</div>

<script>
    $(document).ready(function(){
        $('#machine-list-table td, #machine-list-table th').addClass('py-1 px-2 align-middle');
        $('#machine-list-table').dataTable({
            columnDefs: [
                { orderable: false, targets: 13 }
            ],
        });
    });
</script>
