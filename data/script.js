const CAPTURE = document.getElementById("CAPTURE");
const REFRESH = document.getElementById("REFRESH");



function captureFun() {
    console.log("Foto tomada");
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
}

CAPTURE.addEventListener('click',captureFun);
REFRESH.addEventListener('click', () => location.reload() );