async function saveConfig() {
  const saveBtn = document.getElementById('saveBtn');
  const saveSpinner = document.getElementById('saveSpinner');
  saveSpinner.style.display = 'inline-block';

  const stroomToeslag = document.getElementById('stroom-toeslag').value;
  const stroomBelastingen = document.getElementById('stroom-belastingen').value;
  const tarief = document.getElementById('tarief').value;

  const config = {
    stroomToeslag: parseFloat(stroomToeslag),
    stroomBelastingen: parseFloat(stroomBelastingen),
    btwTarief: parseFloat(tarief)
  };

  const response = await fetch('/save-config', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(config)
  });

  saveSpinner.style.display = 'none';

  if (response.status === 200) {
    alert('Configuration saved successfully');
  } else {
    alert('Error saving configuration');
  }
}

async function readConfig() {
  const readBtn = document.getElementById('readBtn');
  const readSpinner = document.getElementById('readSpinner');
  readSpinner.style.display = 'inline-block';

  const response = await fetch('/read-config');
  
  readSpinner.style.display = 'none';

  if (response.status === 200) {
    const config = await response.json();
    console.log('Configuration:', config);
  } else {
    console.log('Error reading configuration');
  }
}

function updateTime() {
  const dateTime = new Date();
  const options = {
    weekday: 'long',
    year: 'numeric',
    month: 'long',
    day: 'numeric',
    hour: 'numeric',
    minute: 'numeric',
    hour12: false, // Set to false to use 24-hour format
    timeZone: 'Europe/Amsterdam'
  };
  const formattedDateTime = dateTime.toLocaleDateString('en-US', options);
  document.getElementById('date-time').innerHTML = formattedDateTime;
}

updateTime(); // Call initially to set the time
setInterval(updateTime, 60 * 1000);
