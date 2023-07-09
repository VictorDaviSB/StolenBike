// Import the functions you need from the SDKs you need

import { initializeApp } from "firebase/app";

import { getAnalytics } from "firebase/analytics";

import {getDatabase} from "firebase/database";

import {getAuth} from "firebase/auth"

import {useState, useEffect} from 'react'
// TODO: Add SDKs for Firebase products that you want to use

// https://firebase.google.com/docs/web/setup#available-libraries


// Your web app's Firebase configuration

// For Firebase JS SDK v7.20.0 and later, measurementId is optional

const firebaseConfig = {

  apiKey: "AIzaSyDSbBcQLyNha3DfGyfQlI19_5yX5SXt8jU",

  authDomain: "anti-furto-de4a9.firebaseapp.com",

  databaseURL: "https://anti-furto-de4a9-default-rtdb.firebaseio.com",

  projectId: "anti-furto-de4a9",

  storageBucket: "anti-furto-de4a9.appspot.com",

  messagingSenderId: "78648467736",

  appId: "1:78648467736:web:a14a62fb98186afe143e6d",

  measurementId: "G-QHHHC1ZY6Z",

  databaseURL: "https://anti-furto-de4a9-default-rtdb.firebaseio.com/"

};


// Initialize Firebase

export const app = initializeApp(firebaseConfig);

const analytics = getAnalytics(app);

export const database = getDatabase(app);

export const auth = getAuth(app);

export function useAuth(){
  const [currentUser, setCurrentUser] = useState();

  useEffect(() =>{
    onAuthStateChanged(auth, user => setCurrentUser(user))
  },[])
  return currentUser;
}