var update_speed = 1000;

function updateBoardInfo()
{
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function()
    {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200)
        {
            var info = JSON.parse(xmlhttp.responseText);
            document.getElementById("uptime").innerHTML = info.uptime +" seconds";
            document.getElementById("led1_state").style.backgroundColor = (info.l1 == "on") ? "#FF0000" : "#000000";
            document.getElementById("led2_state").style.backgroundColor = (info.l2 == "on") ? "#FFFF00" : "#000000";
            setTimeout(function(){updateBoardInfo();}, update_speed);
        }
    };
    xmlhttp.open("GET","board_info",true);
    xmlhttp.timeout = 3000;
    xmlhttp.ontimeout = function()
    {
        updateBoardInfo();
    }
    xmlhttp.onerror = function(e)
    {
        setTimeout(function(){updateBoardInfo()}, 1000);
    }
    xmlhttp.send();
}

function toggle_led1(){
    var ajaxPost = new XMLHttpRequest;
    ajaxPost.open("GET","led1",true);
    ajaxPost.send("");
}

function toggle_led2(){
    var ajaxPost = new XMLHttpRequest;
    ajaxPost.open("GET","led2",true);
    ajaxPost.send("");
}

function getIp()
{
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function()
    {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200)
        {
            document.getElementById("ip").innerHTML = xmlhttp.responseText;
        }
    }
    xmlhttp.open("GET","ip",true);
    xmlhttp.send();
}

function getMAC()
{
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function()
    {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200)
        {
            document.getElementById("mac").innerHTML = xmlhttp.responseText;
        }
    }
    xmlhttp.open("GET","mac",true);
    xmlhttp.send();
}

