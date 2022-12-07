const express = require('express');
const client_mqtt = require('./mqtt');

const app = express();

const pubTopic = 'TESTE/output'
const subTopic = 'MEDIDOR_ADXL_TESTE'

var msg = '';

client_mqtt.on('connect', () => {
  console.log('Connected')
  client_mqtt.subscribe([subTopic], () => {
    console.log(`Subscribe to topic '${subTopic}'`)
    
  })
  client_mqtt.publish(pubTopic, 'Publicando do NodeJS!!', { qos: 0, retain: false }, (error) => {
    if (error) {
      console.error(error)
    }
  })
})

client_mqtt.on('message', (subTopic, payload) => {
    msg = payload.toString()
    console.log('Received Message: ' + msg + ' from ' + subTopic)
  })

app.listen(3000, ()=>{
    console.log("Servidor Inicializado!");
});

app.get("/", (req, res)=>{ // A barra representa o endpoint, o caminho, que no caso é um endereço padrao
    console.log("Requisição Recebida");

    var msg_split = msg.split(',');
    msg_split = `x: ${msg_split[0]} m/s2; y: ${msg_split[1]} m/s2; z: ${msg_split[2]} m/s2;`
    console.log(msg_split);
    
    res.send(`<html><body><h1>MQTT ESP32 e Acelerômetro ADXL345</h1><h2>O último dado lido é: ${msg_split} </h2></body></html>`);
});