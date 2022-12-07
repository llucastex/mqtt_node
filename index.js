const mqtt = require('mqtt');

const host = 'mqtt.eclipseprojects.io';
const port = '1883';
const connectUrl = `mqtt://${host}:${port}`

var client = mqtt.connect(connectUrl, {
    username: 'ADXL_LUCAS_TESTE_2',
});

const pubTopic = 'TESTE/output'
const subTopic = 'MEDIDOR_ADXL_TESTE'

client.on('connect', () => {
  console.log('Connected')
  client.subscribe([subTopic], () => {
    console.log(`Subscribe to topic '${subTopic}'`)
    
  })
  client.publish(pubTopic, 'Publicando do NodeJS!!', { qos: 0, retain: false }, (error) => {
    if (error) {
      console.error(error)
    }
  })
})

client.on('message', (subTopic, payload) => {
    console.log('Received Message: ' + payload.toString() + ' from ' + subTopic)
  })