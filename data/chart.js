var ctx = document.getElementById("horChart").getContext("2d");
var temperatura, humedad, presion;

obtener("Temperatura");
obtener("Humedad");
obtener("Presion");

var myChart = new Chart(ctx, {
  type: "bar",
  data: {
    labels: ["C°"],
    datasets: [
      {
        label: "Temperatura",
        data: [],
        backgroundColor: [
          "rgba(255, 99, 132, 0.2)",
          "rgba(54, 162, 235, 0.2)",
          "rgba(255, 206, 86, 0.2)",
          "rgba(75, 192, 192, 0.2)",
          "rgba(153, 102, 255, 0.2)",
          "rgba(255, 159, 64, 0.2)",
        ],
        borderColor: [
          "rgba(255, 99, 132, 1)",
          "rgba(54, 162, 235, 1)",
          "rgba(255, 206, 86, 1)",
          "rgba(75, 192, 192, 1)",
          "rgba(153, 102, 255, 1)",
          "rgba(255, 159, 64, 1)",
        ],
        borderWidth: 1,
      },
    ],
  },
  options: {
    indexAxis: "y",
    scales: {
      y: {
        beginAtZero: true,
      },
    },
  },
});

var ctx = document.getElementById("horChart2").getContext("2d");
var myChart2 = new Chart(ctx, {
  type: "bar",
  data: {
    labels: ["hPa"],
    datasets: [
      {
        label: "Presion",
        // data: [1100, 300, 400, 500, 600, 700, 800, 900, 1000],
        data:[],
        backgroundColor: [
          // 'rgba(255, 99, 132, 0.2)',
          "rgba(54, 162, 235, 0.2)",
          // 'rgba(255, 206, 86, 0.2)',
          // 'rgba(75, 192, 192, 0.2)',
          // 'rgba(153, 102, 255, 0.2)',
          // 'rgba(255, 159, 64, 0.2)'
        ],
        borderColor: [
          // 'rgba(255, 99, 132, 1)',
          "rgba(54, 162, 235, 1)",
          "rgba(255, 206, 86, 1)",
          "rgba(75, 192, 192, 1)",
          "rgba(153, 102, 255, 1)",
          "rgba(255, 159, 64, 1)",
        ],
        borderWidth: 1,
      },
    ],
  },
  options: {
    indexAxis: "y",
    scales: {
      y: {
        beginAtZero: false,
      },
      x: {
        beginAtZero: false,
      },
    },
  },
});

var ctx = document.getElementById("horChart3").getContext("2d");
var myChart3 = new Chart(ctx, {
  type: "bar",
  data: {
    labels: ["%"],
    datasets: [
      {
        label: "Humedad Relativa",
        data: [],
        backgroundColor: [
          "rgba(153, 102, 255, 0.2)",
          "rgba(255, 159, 64, 0.2)",
        ],
        borderColor: ["rgba(153, 102, 255, 1)", "rgba(255, 159, 64, 1)"],
        borderWidth: 1,
      },
    ],
  },
  options: {
    indexAxis: "y",
    scales: {
      y: {
        beginAtZero: true,
      },
    },
  },
});

function actualizarGrafico(grafico, variable) {
  var chart;
  if (grafico == 1) {
    chart = myChart;
    variable = temperatura;
  }
  if (grafico == 2) {
    chart = myChart2;
  }
  if (grafico == 3) {
    chart = myChart3;
  }
  chart.data.datasets[0].data.push(variable);

  if (chart.data.labels.length > 10) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }

  chart.update();
}

function obtener(variable) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      if (variable == "Temperatura") {
        temperatura = this.responseText;
        actualizarGrafico(1, temperatura);
      } else if (variable == "Humedad") {
        humedad = this.responseText;
        actualizarGrafico(3, humedad);
      }
      else if (variable == "Presion") {
        presion = this.responseText;
        actualizarGrafico(2, presion);
      }
    }
  };
  xhttp.open("GET", "/obten" + variable, true);
  xhttp.send();
}


function recargarPagina() {
  location.reload();
}

setTimeout(recargarPagina, 600000);
