import React, { useState, useEffect } from 'react'
import { StatusBar } from 'expo-status-bar';
import { Image } from 'react-native';
import { StyleSheet, Text, View } from 'react-native';
import { styles } from './styles';

import {NavigationContainer} from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
//importar firebase
import { app } from './src/config/firebaseConfig';
import { database } from './src/config/firebaseConfig'
import { child, ref, get, Database, getDatabase } from 'firebase/database';
import Boxes from './src/components/Boxes';
import Home from './src/pages/Home';
import Main from './src/pages/Main';
import {enableScreens} from 'react-native-screens'

enableScreens();
const Stack = createNativeStackNavigator();

export default function App() {


  return (
   <NavigationContainer>
    <Stack.Navigator>
      <Stack.Screen
      name='Home'
      component={Home}/>
      <Stack.Screen
      name='Main'
      component={Main}
      />
    </Stack.Navigator>
   </NavigationContainer>
   
  );
}


