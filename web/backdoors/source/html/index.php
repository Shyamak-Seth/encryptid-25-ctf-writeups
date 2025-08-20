<?php
// TODO: remove the backdoor at gV82gCVr46xy6lxryz9sLkvk.php
try {
    if (isset($_GET['page'])) {
        if (preg_match('/\.php/', $_GET['page'])) {
            $page = $_GET['page'];

            if (basename($page) === basename(__FILE__) && strpos($page, "php://filter") === false) {
                throw new Exception("Self-inclusion is not allowed.");
            }
            else if (strlen($page) > 150) {
                throw new Exception("File name too long.");
            }
            else {
                $result = @include($page);
                if ($result === false) {
                    throw new Exception("404 not found.");
                }
            }
        } else { ?>
<h1>Forbidden</h1>
<p>The file name must end with .php</p>
<?php 
        }
    } else {
?>
<h1>Welcome to our blog!</h1>
<p>There's nothing useful here - it's just an innocent little page. But, take some cute bunny pics along the way:</p>
<img src="/static/1.jpg" style="width: 31vw; height: 31vw; object-fit: cover;">
<img src="/static/2.jpg" style="width: 31vw; height: 31vw; object-fit: cover;">
<img src="/static/3.jpg" style="width: 31vw; height: 31vw; object-fit: cover;">
<img src="/static/4.jpg" style="width: 31vw; height: 31vw; object-fit: cover;">
<img src="/static/5.jpg" style="width: 31vw; height: 31vw; object-fit: cover;">
<img src="/static/6.jpg" style="width: 31vw; height: 31vw; object-fit: cover;"><br><br><br>
<h2 style="text-align: center">Useful Links</h3>
<p style="text-align: center"><a href="/?page=page1.php">Page 1</a></p>
<p style="text-align: center"><a href="/?page=page2.php">Page 2</a></p>
<p style="text-align: center"><a href="/?page=page3.php">Page 3</a></p>
<?php 
    }
} catch (Exception $e) {
    http_response_code(404);
    echo "<h1>Error</h1>";
    echo "<p>" . htmlspecialchars($e->getMessage()) . "</p>";
}
?>
