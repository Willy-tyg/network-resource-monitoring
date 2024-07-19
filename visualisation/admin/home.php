

<h1>Welcome to <?php echo $_settings->info('name') ?> - Dashboard</h1>
<hr class="border-purple">
<style>
    #website-cover {
        width: 100%;
        height: 30em;
        object-fit: cover;
        object-position: center center;
    }
</style>


<div class="row">
    <div class="col-12 col-sm-12 col-md-6 col-lg-3">
        <div class="info-box bg-gradient-light shadow">
            <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-laptop"></i></span>
            <div class="info-box-content">
                <span class="info-box-text">Ordinateur</span>
                <span class="info-box-number text-right">
                    <?php 
                    echo $conn->query("SELECT * FROM `SystemInfo`")->num_rows;
                    ?>
                </span>
            </div>
        </div>
    </div>

    <div class="col-12 col-sm-12 col-md-6 col-lg-3">
    <div class="info-box bg-gradient-light shadow">
        <span class="info-box-icon bg-gradient-warning elevation-1"><i class="fas fa-memory"></i></span>
        <div class="info-box-content">
            <span class="info-box-text">RAM TOTAL</span>
            <span class="info-box-number text-right">
                <?php 
                    // Exécution de la requête
                    $result = $conn->query("SELECT SUM(ram_total) AS total_ram FROM SystemInfo");

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
                    $result = $conn->query("SELECT AVG(ram_free) AS avg_ram FROM SystemInfoVariable");

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
                        $result = $conn->query("SELECT SUM(disk_total) AS total_disk FROM SystemInfo");

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
                    $result = $conn->query("SELECT AVG(disk_unused) AS avg_disk FROM SystemInfoVariable");

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

