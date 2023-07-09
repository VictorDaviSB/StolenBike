import React from 'react'
import { View, StyleSheet,Text,Image } from 'react-native'
import logodistancia from '../img/distancia.png'
import logovelocidade from "../img/painel-de-controle.png"
const Boxes = (props) => {
  return (
    <View style={styles.container}>
        <View style={styles.box}>
            <View style={styles.inner}>
                <Image style={styles.icon} source={logodistancia}/>
            <Text style={styles.texto}>A distância entre você e a bicicleta é de {Math.floor(props.distancia)} metros</Text>
            </View>
        </View>
        <View style={styles.box}>
            <View style={styles.inner}>
            <Image style={styles.icon} source={logovelocidade}/>

            <Text style={styles.texto}>A sua bicicleta esta se movimentando a {props.velocidade} km/h</Text>
            </View>
        </View>
    </View>
  )
}

const styles = StyleSheet.create({
    container:{
        width:'100%',
        height: '40%',
        padding: 5,
        flexDirection: 'row',
        flexWrap: 'wrap'
        
    },
    box: {
        width: '50%',
        height: '50%',
        padding: 5
    },
    inner: {
        flex: 1,
       backgroundColor: '#eee',
        alignItems: 'center',
        justifyContent: 'center',
        borderRadius: 15
    },
    icon:{
        width:50,
        height:50,
        
        
    },
    texto:{
        fontFamily: 'sans-serif'
    }
})

export default Boxes