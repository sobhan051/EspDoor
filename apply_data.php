<?php
    $hostname = "localhost";
    $username = "root" ;
    $password = "";
    $databsename = "door";
    $tag = "" ; $room = "" ; $class = ""; $day = 0;
    date_default_timezone_set("Asia/Tehran");
    $nowTime = date("H:i:s") ;
    // $nowDate = date("Y-m-d");

    switch(date("l")){
        case "Saturday": $day = 1;break;
        case "Sunday" : $day = 2;break;
        case "Monday": $day = 3;break;
        case "Tuesday" : $day = 4;break;
        case "Wednesday": $day = 5;break;
        case "Thursday": $day = 6;break;
        case "Friday" : $day = 7;break;
    }
    function gregorian_to_jalali($gy, $gm, $gd, $mod = '/') {
        $g_d_m = array(0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334);
        $gy2 = ($gm > 2) ? ($gy + 1) : $gy;
        $days = 355666 + (365 * $gy) + ((int) (($gy2 + 3) / 4)) - ((int) (($gy2 + 99) / 100)) + ((int) (($gy2 + 399) / 400)) + $gd + $g_d_m[$gm - 1];
        $jy = -1595 + (33 * ((int) ($days / 12053)));
        $days %= 12053;
        $jy += 4 * ((int) ($days / 1461));
        $days %= 1461;
        if ($days > 365) {
        $jy += (int) (($days - 1) / 365);
        $days = ($days - 1) % 365;
        }
        if ($days < 186) {
        $jm = 1 + (int) ($days / 31);
        $jd = 1 + ($days % 31);
        } else {
        $jm = 7 + (int) (($days - 186) / 30);
        $jd = 1 + (($days - 186) % 30);
        }
        return ($mod == '') ? array($jy, $jm, $jd) : $jy . $mod . $jm . $mod . $jd;
    }
    $nowDate = gregorian_to_jalali(date("Y"), date("m"), date("d"));
    $con = mysqli_connect($hostname , $username , $password , $databsename);
    if($con->connect_error)
        die("Connection Failed:".$con->connect_error);

    if(isset($_POST["UIDCard"]) && isset($_POST["Room"])) {
        $tag = trim($_POST["UIDCard"]);
        $room = trim($_POST["Room"]);
    }

    $sql = "SELECT * FROM class WHERE (`uid` = '$tag' and `class_day` = '$day' and `class_number` = '$room' and '$nowTime' BETWEEN `class_start_time` and `class_end_time`)";
    $result = $con->query($sql);

    if($result->num_rows > 0){
    
        $rows = $result->fetch_assoc();
        $first_name = $rows["first_name"];
        $last_name = $rows["last_name"];
        $student_id = $rows["student_id"];
        $sql = "INSERT INTO log (first_name, last_name, student_id , log_date, log_time, class_number ,authorised) VALUES ('$first_name', '$last_name', '$student_id', '$nowDate', '$nowTime','$room' ,1)";
        $result = $con->query($sql);
        echo "1";
    }else{
        $sql = "SELECT * FROM class WHERE `uid` = '$tag'";
        $result = $con->query($sql);
        if($result->num_rows > 0){
            $rows = $result->fetch_assoc();
            $first_name = $rows["first_name"];
            $last_name = $rows["last_name"];
            $student_id = $rows["student_id"];
        }else{
            $first_name = "NULL";
            $last_name = "NULL";
            $student_id = 0;
        }
        $sql = "INSERT INTO log (`student_id`, `first_name`, `last_name` , `log_date`, `log_time`, `class_number` ,`authorised`) VALUES ('$student_id','$first_name', '$last_name',  '$nowDate', '$nowTime', '$room', 0)";
        $con->query($sql);
        echo "0";
    }

    $con->close();
?>
