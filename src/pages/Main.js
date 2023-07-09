import React, { useState, useEffect , useRef} from 'react'
import { StatusBar } from 'expo-status-bar';
import { Image, Platform } from 'react-native';
import { StyleSheet, Text, View,Button } from 'react-native';
import { styles } from './styles';
import MapView, { Marker } from "react-native-maps";
import {
  requestForegroundPermissionsAsync,
  getCurrentPositionAsync,
  LocationObject,
  watchPositionAsync,
  LocationAccuracy
} from 'expo-location'

//importar firebase
import { database } from '../config/firebaseConfig'
import { child, ref, get, Database, getDatabase, set } from 'firebase/database';
import * as Device from 'expo-device';
import * as Notifications from 'expo-notifications';
import { subscribeNotification } from '../Functions/Notification';


import Boxes from '../components/Boxes';

Notifications.setNotificationHandler({
  handleNotification: async () => ({
    shouldShowAlert: true,
    shouldPlaySound: false,
    shouldSetBadge: false,
  }),
});
var furtada = false;

subscribeNotification();
const Main = () => {
  const [location, setLocation] = useState(null);
  const [locationBike, setLocationBike] = useState({ latitude: 0, longitude: 0 });
  const [velocidade, setVelocidade] = useState(0);

  const dbRef = ref(database);

  const [distancia, setDistancia] = useState(0);

  async function requestLocationPermission() {
    const { granted } = await requestForegroundPermissionsAsync();

    if (granted) {
      const currentPosition = await getCurrentPositionAsync();
      setLocation(currentPosition);
      console.log(" Localização Atual =>", currentPosition);
    }
  }

  useEffect(() => {
    requestLocationPermission();
    
  }, []);

  useEffect(() =>{
    async function getBikeLocation() {

      await get(child(dbRef, `GPS`)).then((snapshot) => {
        if (snapshot.exists()) {
          console.log(snapshot.val());
          setLocationBike({
            latitude: snapshot.val().f_latitude,
            longitude: snapshot.val().f_longitude
          })
          setVelocidade(snapshot.val().velocidade)
          console.log(locationBike);

        } else {
          console.log("No data available");
        }
      }).catch((error) => {
        console.error(error);
      });
    }
    getBikeLocation();
  },[distancia])

  useEffect(() =>{
    async function sendDistanceData(){
      set(ref(database, 'GPS/distanciaTotal'),
        Math.floor(distancia)
      )
      console.log('====================================');
      console.log("Enviado");
      console.log('====================================');
    }
    sendDistanceData();
  },[distancia])
  //verificar a posição do celular do usuário
  useEffect(() => {
    watchPositionAsync({
      accuracy: LocationAccuracy.Highest,
      timeInterval: 1000,
      distanceInterval: 1
    }, (response) => {
      setLocation(response);

    })
  }, [])

  useEffect(() => {
    if (location) {

      let a = (location.coords.latitude - locationBike?.latitude) * 111.1;
      let b = (location.coords.longitude - locationBike?.longitude) * 96.2;
      let distanciaAbs = Math.sqrt(a * a + b * b) * 1000;
      setDistancia(distanciaAbs);
    }
  })


  const [expoPushToken, setExpoPushToken] = useState('');
  const [notification, setNotification] = useState(false);
  const notificationListener = useRef();
  const responseListener = useRef();
  //Enviar a notificação
  useEffect(() => {
    registerForPushNotificationsAsync().then(token => setExpoPushToken(token));

    notificationListener.current = Notifications.addNotificationReceivedListener(notification => {
      setNotification(notification);
    });

    responseListener.current = Notifications.addNotificationResponseReceivedListener(response => {
      console.log("foreground");
      
    });

    return () => {
      Notifications.removeNotificationSubscription(notificationListener.current);
      Notifications.removeNotificationSubscription(responseListener.current);
    };
  }, []);


  //Trigger na função de enviar notificação caso as condições de velocidade e distancia forem atendidas
  useEffect(() =>{
    if (distancia > 30 && velocidade > 4) {
      if (!furtada) {
        schedulePushNotification();
      }
      
      furtada = true;
    }
    else{
      furtada = false;
    }
  },[distancia])
  return (
    <View style={styles.container}>

      {

        (location && locationBike.latitude != 0) &&
        <MapView style={styles.map}
          initialRegion={{
            latitude: location.coords.latitude,
            longitude: location.coords.longitude,
            latitudeDelta: 0.005,
            longitudeDelta: 0.005
          }}
        >

          <Marker
            pinColor="green"
            description='Sua localização'
            coordinate={{
              latitude: location.coords.latitude,
              longitude: location.coords.longitude
            }} />



          <Marker
            
            coordinate={{
              latitude: locationBike.latitude,
              longitude: locationBike.longitude
            }}
            >
            <Image
              source={require('../img/bicycle.png')}
              style={styles.markerImage}
            />
          </Marker>


        </MapView>

      }
      <Boxes distancia={distancia} velocidade={velocidade}/>
        
    
    </View>
  );
}



async function schedulePushNotification() {
  console.log("Cheguei aqui");

  await Notifications.scheduleNotificationAsync({
    content: {
      title: "Alerta!!!",
      body: 'Sua bicicleta está se movendo!',
      data: { },
    },
    trigger: { seconds: 2 },
  });
}

async function registerForPushNotificationsAsync() {
  let token;

  if (Platform.OS === 'android') {
    await Notifications.setNotificationChannelAsync('default', {
      name: 'default',
      importance: Notifications.AndroidImportance.MAX,
      vibrationPattern: [0, 250, 250, 250],
      lightColor: '#FF231F7C',
    });
  }

  if (Device.isDevice) {
    const { status: existingStatus } = await Notifications.getPermissionsAsync();
    let finalStatus = existingStatus;
    if (existingStatus !== 'granted') {
      const { status } = await Notifications.requestPermissionsAsync();
      finalStatus = status;
    }
    if (finalStatus !== 'granted') {
      alert('Failed to get push token for push notification!');
      return;
    }
    token = (await Notifications.getExpoPushTokenAsync()).data;
    console.log(token);
  } else {
    alert('Must use physical device for Push Notifications');
  }

  return token;
}

export default Main;