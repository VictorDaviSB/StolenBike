import * as Notifications from 'expo-notifications';

export function subscribeNotification() {
  console.log('subscribing')
  Notifications.addNotificationReceivedListener((response)=>console.log(""))
  Notifications.addNotificationResponseReceivedListener((response)=>console.log('background'))
}