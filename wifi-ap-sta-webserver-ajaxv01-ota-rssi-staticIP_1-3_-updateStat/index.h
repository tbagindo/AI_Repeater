static const char MAIN_PAGE[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
      .switch {
          position: relative;
          display: inline-block;
          width: 60px;
          height: 26px;
       }
        
      .switch input { 
          opacity: 0;
          width: 0;
          height: 0;
       }

      .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        -webkit-transition: .4s;
        transition: .4s;
      }

      .slider:before {
        position: absolute;
        content: "";
        height: 18px;
        width: 18px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: .4s;
        transition: .4s;
      }

      input:checked + .slider {
          background-color: #2196F3;
      }

      input:focus + .slider {
          box-shadow: 0 0 1px #2196F3;
      }

      input:checked + .slider:before {
          -webkit-transform: translateX(26px);
          -ms-transform: translateX(26px);
           transform: translateX(34px);
      }

      /* Rounded sliders */
      .slider.round {
          border-radius: 26px;
      }

      .slider.round:before {
          border-radius: 50%;
      }
      html {
          font-family: Arial;
          display: inline-block;
          margin: 0px auto;
          text-align: center;
      }
      h2 { font-size: 2.0rem; }
      p { font-size: 2.0rem; }
      .units { font-size: 1.0rem; }
      .dht-labels{
        font-size: 1.0rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
      .WiFi-labels{
        font-size: 1.0rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
    </style>
    <title>ApaITU ESP01 SmartSwitch</title>
  </head>
  <body>
    <h2>ApaITU SmartSwitch</h2>
    <p>
      <img src="data:image/jpeg;base64,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" alt="" />
    </p>
    <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
      <span class="dht-labels">Temperature</span> 
      <span class="dht-labels"  id="temperature">0</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      
      <i class="fas fa-tint" style="color:#00add6;"></i> 
      <span class="dht-labels">Humidity</span>
      <span class="dht-labels"  id="humidity">0</span>
      <sup class="units">%</sup>
    </p>
    <p>
      <i class="fas fa-wifi" style="color:#00add6;"></i>
      <span class="WiFi-labels">SSID</span>
      <span id="ssid"></span>
      <sup class="units"></sup>
    </p>
    <p>
      <i class="fas fa-wifi" style="color:#00add6;"></i>
      <span class="WiFi-labels">RSSI</span>
      <span id="rssi">0</span>
      <sup class="units">dbi</sup>
    </p>
    <p>  
        <i class="far fa-lightbulb" style="color:#00add6;"></i> 
        <span class="dht-labels">Saklar Lampu</span>
        <label class="switch">    
          <input type="checkbox" id="led" onchange="ledButton('led')" @@LEDSTATE@@>
          <span class="slider"></span>
        </label>
    </p>

  </body>
  <script>
  function ledButton(id){
    const button = document.getElementById(id);
    if(button.checked){
      var url = "/on";
    }else{
      var url = "/off";
    }
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange= function(){
      if(this.readyState==4 && this.status==200){
        document.getElementById("status").innerHTML = this.responseText
      }
    };
    xhttp.open("GET",  url, true);
    xhttp.send();
  }
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("temperature").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/Temp", true);
    xhttp.send();
  }, 4000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("humidity").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/Humid", true);
    xhttp.send();
  }, 4000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("rssi").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/getRSSI", true);
    xhttp.send();
  }, 4000 ) ;
  
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("ssid").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/getSSID", true);
    xhttp.send();
  }, 4000 ) ;
 
  setInterval(function  () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var inputChecked;
        if( this.responseText == 1){ 
            inputChecked = true;
        }else{ 
            inputChecked = false;
        }
        document.getElementById("led").checked = inputChecked;
      }     
    };
    xhttp.open("GET", "/state", true);
    xhttp.send();
  }, 1000 ) ;
  </script>
</html>
)==";
