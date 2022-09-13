window.addEventListener('load', getStates);

function getStates(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        for (i in myObj.gpios) {
          var output = myObj.gpios[i].output;
          var state = myObj.gpios[i].state;
          console.log(output);
          console.log(state);
          if (state == "1") {
            document.getElementById(output).checked = true;
            document.getElementById(output+"s").innerHTML = "ON";
          }
          else {
            document.getElementById(output).checked = false;
            document.getElementById(output+"s").innerHTML = "OFF";
          }
        }
      }
    }; 
    xhr.open("GET", "/states", true);
    xhr.send();
  } 


  function toggleCheckbox (element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) {
      xhr.open("GET", "/update?output="+element.id+"&state=1", true);
      document.getElementById(element.id+"s").innerHTML = "ON";
    }
    else {
      xhr.open("GET", "/update?output="+element.id+"&state=0", true);
      document.getElementById(element.id+"s").innerHTML = "OFF"; 
    }
    xhr.send();
  }