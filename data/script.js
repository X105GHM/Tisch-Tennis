var playerNames = { player1: "Player1", player2: "Player2" };
var scores = { player1: 0, player2: 0 };

document.addEventListener("DOMContentLoaded", function() {
    showSection('start');

    document.getElementById('nameForm').addEventListener('submit', handleNames);
    document.getElementById('toggleDisplay').addEventListener('change', updateDisplayStatus);
});

function updateDisplayStatus() {
    var toggle = document.getElementById('toggleDisplay');
    var isActive = toggle.checked ? "1" : "0";
    document.getElementById('toggleLabel').innerText = toggle.checked ? "Anzeige aktiv" : "Anzeige inaktiv";
    
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/setDisplayActive?active=" + isActive, true);
    xhr.send();
}

function showSection(sectionId) {
    document.querySelectorAll('.section').forEach(function(section) {
        section.style.display = 'none';
    });
    document.getElementById(sectionId).style.display = 'block';

    if (sectionId === "gamelist") {
        loadGameHistory();
    } else if (sectionId === "tierlist") {
        loadTierList();
    }
}

function handleNames(event) {
    event.preventDefault();
    var p1 = document.getElementById('player1Input').value.trim();
    var p2 = document.getElementById('player2Input').value.trim();
    if (p1 === "" || p2 === "") {
        alert("Bitte beide Namen eingeben!");
        return;
    }

    playerNames.player1 = p1;
    playerNames.player2 = p2;

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/setNames?player1=" + encodeURIComponent(p1) + "&player2=" + encodeURIComponent(p2), true);
    xhr.onload = function() {
        if (xhr.status === 200) {
            document.getElementById('player1Name').innerText = p1;
            document.getElementById('player2Name').innerText = p2;
            document.getElementById('gameHeading').innerText = p1 + " vs. " + p2;
            document.getElementById('nameInputDiv').style.display = 'none';
            document.getElementById('scoreDiv').style.display = 'block';
        } else {
            alert("Fehler beim Setzen der Spielernamen");
        }
    };
    xhr.send();
}

function updateScore(id, inc) {
    if (id === 1) {
        scores.player1 += inc;
        if (scores.player1 < 0) scores.player1 = 0;
        document.getElementById('score1').innerText = scores.player1;
    } else if (id === 2) {
        scores.player2 += inc;
        if (scores.player2 < 0) scores.player2 = 0;
        document.getElementById('score2').innerText = scores.player2;
    }

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/updateScore?id=" + id + "&inc=" + inc, true);
    xhr.onload = function() {
        console.log("Server response: " + xhr.responseText);
        try {
            var resp = JSON.parse(xhr.responseText);
            if (resp.status === "finished") {
                console.log("Spiel beendet (via Serverantwort).");
                finishGame();
            }
        } catch (e) {
            console.log("Konnte die Serverantwort nicht parsen: " + xhr.responseText);
        }
    };
    xhr.send();

    console.log("Punkte aktualisiert: " + playerNames.player1 + " " + scores.player1 + " : " +
                playerNames.player2 + " " + scores.player2);
}

function finishGame() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/finishGame?force=true", true);
    xhr.onload = function() {
        if (xhr.status === 200) {
            resetUiAfterGame();
        }
    };
    xhr.send();
}

function resetUiAfterGame() {
    scores = { player1: 0, player2: 0 };
    document.getElementById('score1').innerText = "0";
    document.getElementById('score2').innerText = "0";
    document.getElementById('nameInputDiv').style.display = 'block';
    document.getElementById('scoreDiv').style.display = 'none';
    showSection('gamelist');
    updateDisplayStatus();
}

function loadGameHistory() {
    fetch("/getGameHistory")
        .then(function(response) {
            return response.json();
        })
        .then(function(data) {
            var container = document.getElementById("gameHistory");
            container.innerHTML = "";
            if (data.length === 0) {
                container.innerText = "Hier gibt es noch nichts zu sehen.";
                return;
            }
            data.forEach(function(game) {
                var div = document.createElement("div");
                div.innerText = game.player1 + " (" + game.score1 + ") vs. " +
                                game.player2 + " (" + game.score2 + ")";
                container.appendChild(div);
            });
        })
        .catch(function() {
            document.getElementById("gameHistory").innerText = "Fehler beim Laden der Spielliste.";
        });
}

function loadTierList() {
    fetch("/getTierList")
        .then(function(response) {
            return response.json();
        })
        .then(function(data) {
            data.sort(function(a, b) {
                return b.wins - a.wins;
            });
            var container = document.getElementById("tierList");
            container.innerHTML = "";
            if (data.length === 0) {
                container.innerText = "Hier gibt es noch nichts zu sehen.";
                return;
            }
            data.forEach(function(player) {
                var div = document.createElement("div");
                div.innerText = player.name + ": " + player.wins + " Siege, " + player.losses + " Niederlagen";
                container.appendChild(div);
            });
        })
        .catch(function() {
            document.getElementById("tierList").innerText = "Fehler beim Laden der Tierlist.";
        });
}

setInterval(function() {
    loadGameHistory();
    loadTierList();
}, 3000);
