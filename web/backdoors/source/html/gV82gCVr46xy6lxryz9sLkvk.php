<?php
try {
    $actualKey = trim(file_get_contents('/opt/super_secret_key.txt'));
    if (!(isset($_GET['dir']) && isset($_GET['key']))) {
        echo "<p>Error: Missing parameters.</p>";
    } else {
        $dir = $_GET['dir'];
        $key = $_GET['key']; 
        if ($key !== $actualKey) {
            echo "<p>Error: Incorrect credentials.";
        } else if (is_dir($dir)) {
            if ($dh = opendir($dir)) {
                echo "<h1>File Browser Backdoor</h1>";
                echo "<h2>Files in directory: $dir</h2><ul>";
                while (($file = readdir($dh)) !== false) {
                    if ($file !== '.' && $file !== '..') {
                        echo "<li>$file</li>";
                    }
                }
                echo "</ul>";
                closedir($dh);
            } else {
                echo "Unable to open directory.";
            }
        } else {
            echo "Not a valid directory.";
        }
    } 
} catch (Exception) {
    echo "There was an error.";
}
?>
