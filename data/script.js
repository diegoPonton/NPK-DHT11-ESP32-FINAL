// Get current sensor readings when the page loads
window.addEventListener('load', getReadings);
//Function to add date and time of last update
function updateDateTime() {
 var currentdate = new Date();
 var datetime = currentdate.getDate() + "/"
 + (currentdate.getMonth()+1) + "/"
 + currentdate.getFullYear() + " at "
 + currentdate.getHours() + ":"
 + currentdate.getMinutes() + ":"
 + currentdate.getSeconds();
 document.getElementById("update-time").innerHTML = datetime;
 console.log(datetime);
}
// Function to get current readings on the web page when it loads at first
function getReadings() {
 var xhr = new XMLHttpRequest();
 xhr.onreadystatechange = function() {
 if (this.readyState == 4 && this.status == 200) {
 var myObj = JSON.parse(this.responseText);
 console.log(myObj);
 document.getElementById("temperature").innerHTML = myObj.temperature;
 document.getElementById("humidity").innerHTML = myObj.humidity;
 document.getElementById("nitrogen").innerHTML = myObj.nitrogen;
 document.getElementById("potassium").innerHTML = myObj.potassium;
 document.getElementById("phosphorus").innerHTML = myObj.phosphorus;
 updateDateTime();
 }
 };
 xhr.open("GET", "/readings", true);
 xhr.send();
}
// Create an Event Source to listen for events
if (!!window.EventSource) {
  var source = new EventSource('/events');
 source.addEventListener('open', function(e) {
 console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
 if (e.target.readyState != EventSource.OPEN) {
 console.log("Events Disconnected");
 }
 }, false);
 source.addEventListener('new_readings', function(e) {
 console.log("new_readings", e.data);
 var obj = JSON.parse(e.data);
 document.getElementById("temperature").innerHTML = obj.temperature;
 document.getElementById("humidity").innerHTML = obj.humidity;
 document.getElementById("nitrogen").innerHTML = obj.nitrogen;
 document.getElementById("potassium").innerHTML = obj.potassium;
 document.getElementById("phosphorus").innerHTML = obj.phosphorus;
 updateDateTime();
 }, false);
}
