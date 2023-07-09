import React, { useState } from 'react'
import { styles } from './styles';
import {View, Text, Button, TextInput, Alert, StyleSheet} from 'react-native'
import {signInWithEmailAndPassword} from 'firebase/auth'
import { auth } from '../config/firebaseConfig';
 
const Home = ({navigation}) =>{

    const [loginEmail,setLoginEmail] = useState("");
    const [loginPassword,setLoginPassword] = useState("");


    const login = async () =>{
        try{
            const user = await signInWithEmailAndPassword(auth
                , loginEmail,
                loginPassword
                ).then(() =>{
                    navigation.navigate("Main")
                }).catch(err => 
                    Alert(err.message));
        }catch(err){
            console.log('====================================');
            console.log(err);
            console.log('====================================');
        }
    };
   
    return (
        <View style={styles.container}>
            <Text style={HomeStyle.Title}>SISTEMA ANTI-FURTO BIKE</Text>
            <Text>Email</Text>
        <TextInput style={HomeStyle.input} value={loginEmail} onChangeText={setLoginEmail}>

        </TextInput>
        <Text>Senha</Text>
        <TextInput style={HomeStyle.input} value={loginPassword} onChangeText={setLoginPassword}/>

        
        <Button title='Entrar' onPress={login}/>
        </View>
    )
}

const HomeStyle = StyleSheet.create({
    input: {
      height: 40,
      margin: 12,
      width: '60%',
      borderWidth: 1,
      padding: 10,
    },
    Title:{
        fontSize: 28,
        fontFamily: 'sans-serif-light',
        padding: 10,
        borderStyle: 'solid',
        borderWidth: 3,
        marginBottom: 20

    },
    container: {
        justifyContent:'center',

    }
  });
export default Home;