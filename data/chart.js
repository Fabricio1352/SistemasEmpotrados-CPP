
var ctx = document.getElementById('horChart').getContext('2d');
// ctx.canvas.width = 10; // Ancho del lienzo
// ctx.canvas.height = 1; // Alto del lienzo
var myChart = new Chart(ctx, {
    type: 'bar',
    data: {
        labels: ['C°'],
        datasets: [{
          label: 'Temperatura',
          data: [-10,10,20,30,40,50], // esto es lo que se va sacar del html tmb
            backgroundColor: [
                'rgba(255, 99, 132, 0.2)',
                'rgba(54, 162, 235, 0.2)',
                'rgba(255, 206, 86, 0.2)',
                'rgba(75, 192, 192, 0.2)',
                'rgba(153, 102, 255, 0.2)',
                'rgba(255, 159, 64, 0.2)'
            ],
            borderColor: [
                'rgba(255, 99, 132, 1)',
                'rgba(54, 162, 235, 1)',
                'rgba(255, 206, 86, 1)',
                'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)'
            ],
            borderWidth: 1
        }]
    },
    options: {
      indexAxis:'y',
        scales: {
            y: {
              
                beginAtZero: true            },
            x:{
                beginAtZero: true
                
            }
        }
    }
});


var ctx = document.getElementById('horChart2').getContext('2d');
var myChart2 = new Chart(ctx, {
    type: 'bar',
    data: {
        labels: ['hPa'],
        datasets: [{
          label: 'Presion',
          data: [1100,300,400,500,600,700,800,900,1000], // esto es lo que se va sacar del html tmb
            backgroundColor: [
                // 'rgba(255, 99, 132, 0.2)',
                'rgba(54, 162, 235, 0.2)',
                // 'rgba(255, 206, 86, 0.2)',
                // 'rgba(75, 192, 192, 0.2)',
                // 'rgba(153, 102, 255, 0.2)',
                // 'rgba(255, 159, 64, 0.2)'
            ],
            borderColor: [
                // 'rgba(255, 99, 132, 1)',
                'rgba(54, 162, 235, 1)',
                'rgba(255, 206, 86, 1)',
                'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)'
            ],
            borderWidth: 1
        }]
    },
    options: {
      indexAxis:'y',
        scales: {
            
            y: {
                
                beginAtZero: false
            },
            x:{
                beginAtZero : false
            }
        }
    }
});



var ctx = document.getElementById('horChart3').getContext('2d');
var myChart3 = new Chart(ctx, {
    type: 'bar',
    data: {
        labels: ['%'],
        datasets: [{
          label: 'Humedad Relativa',
          data: [100], // esto es lo que se va sacar del html tmb
            backgroundColor: [
                // 'rgba(255, 99, 132, 0.2)',
                // 'rgba(54, 162, 235, 0.2)',
                // 'rgba(255, 206, 86, 0.2)',
                // 'rgba(75, 192, 192, 0.2)',
                'rgba(153, 102, 255, 0.2)',
                'rgba(255, 159, 64, 0.2)'
            ],
            borderColor: [
                // 'rgba(255, 99, 132, 1)',
                // 'rgba(54, 162, 235, 1)',
                // 'rgba(255, 206, 86, 1)',
                // 'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)'
            ],
            borderWidth: 1
        }]
    },
    options: {
      indexAxis:'y',
        scales: {
            y: {
              
                beginAtZero: true
                
            }
        }
    }
});


