

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

<h4>Analyse du PC avec pour adresse MAC   : <?php echo $mac_address ?></h4>

<style>
    #website-cover {
        width: 100%;
        height: 30em;
        object-fit: cover;
        object-position: center center;
    }
</style>


    <div class="col-12 col-sm-12 col-md-6 col-lg-3">
    <div class="info-box bg-gradient-light shadow">
        <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-memory"></i></span>
        <div class="info-box-content">
            <span class="info-box-text">RAM TOTAL</span>
            <span class="info-box-number text-right">
                <?php 
                    // Exécution de la requête
                    $result = $conn->query("SELECT ram_total AS total_ram FROM SystemInfo where mac_address = '{$_GET['mac_address']}'");

                    if ($result) {
                        $row = $result->fetch_assoc();
                        $totalRamInBytes = $row['total_ram'];
                        $totalRamInGigabytes = $totalRamInBytes / (1024 * 1024);
                        $totalRamInGigabytesRounded = round($totalRamInGigabytes);
                        echo $totalRamInGigabytesRounded . ' Go';
                    } else {
                        echo "Error: " . $conn->error;
                    }
                ?>
            </span>
        </div>
    </div>



</div>
<div class="col-12 col-sm-12 col-md-6 col-lg-3">
    <div class="info-box bg-gradient-light shadow">
        <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-memory"></i></span>
        <div class="info-box-content">
            <span class="info-box-text">RAM FREE MOY</span>
            <span class="info-box-number text-right">
                <?php 
                    // Exécution de la requête
                    $result = $conn->query("SELECT AVG(ram_free) AS avg_ram FROM SystemInfoVariable where mac_address = '{$_GET['mac_address']}'");

                    if ($result) {
                        $row = $result->fetch_assoc();
                        $avgRamInBytes = $row['avg_ram'];
                        $avgRamInGigabytes = $avgRamInBytes / (1024 * 1024 * 1024); // Convertir en Go
                        $avgRamInGigabytesRounded = round($avgRamInGigabytes, 2); // Arrondir à 2 décimales
                        echo $avgRamInGigabytesRounded . ' Go';
                    } else {
                        echo "Error: " . $conn->error;
                    }
                ?>
            </span>
        </div>
    </div>
</div>

    <div class="col-12 col-sm-12 col-md-6 col-lg-3">

        <div class="info-box bg-gradient-light shadow">
            <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-memory"></i></span>
            <div class="info-box-content">
                <span class="info-box-text">DD TOTAL</span>
                <span class="info-box-number text-right">
                    <?php 
                        // Exécution de la requête
                        $result = $conn->query("SELECT disk_total AS total_disk FROM SystemInfo where mac_address = '{$_GET['mac_address']}'");

                        if ($result) {
                            $row = $result->fetch_assoc();
                            $totalRamInBytes = $row['total_disk'];
                            $totalRamInGigabytes = $totalRamInBytes / (1024 * 1024 *1024 );
                            $totalRamInGigabytesRounded = round($totalRamInGigabytes);

                            echo  $totalRamInGigabytesRounded . ' Go';
                        } else {
                            echo "Error: " . $conn->error;
                        }
                    ?>
                </span>
            </div>
        </div>
       
    </div>

   
</div>

<div class="col-12 col-sm-12 col-md-6 col-lg-3">
    <div class="info-box bg-gradient-light shadow">
        <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-memory"></i></span>
        <div class="info-box-content">
            <span class="info-box-text">DISQUE FREE MOY</span>
            <span class="info-box-number text-right">
                <?php 
                    // Exécution de la requête
                    $result = $conn->query("SELECT AVG(disk_unused) AS avg_disk FROM SystemInfoVariable where mac_address = '{$_GET['mac_address']}'");

                    if ($result) {
                        $row = $result->fetch_assoc();
                        $avgRamInBytes = $row['avg_disk'];
                        $avgRamInGigabytes = $avgRamInBytes / (1024 * 1024 * 1024); // Convertir en Go
                        $avgRamInGigabytesRounded = round($avgRamInGigabytes, 2); // Arrondir à 2 décimales
                        echo $avgRamInGigabytesRounded . ' Go';
                    } else {
                        echo "Error: " . $conn->error;
                    }
                ?>
            </span>
        </div>
    </div>
</div>

<hr>
