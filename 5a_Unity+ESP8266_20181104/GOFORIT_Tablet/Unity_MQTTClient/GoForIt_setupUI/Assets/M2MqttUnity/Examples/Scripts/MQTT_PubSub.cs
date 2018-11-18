/*********************************************************************
 * MQTT_PubSub.cs
 * GO FOR IT - Swarm hats setup tool
 * Trigger lights on hat using MQTT
 * mail@janfiess.com, fall 2018
 *
 * This script is attached to the Manager GameObject
 * Connecting to MQTT Broker
 * Publishing + Subscribing (to) MQTT messages to the MQTT broker when UI buttons pressed
 * MQTTBehaviour is MQTT backend, also attached to Manager GameObject
 *********************************************************************/


/*
The MIT License (MIT)

Copyright (c) 2018 Giovanni Paolo Vigano'

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;
using M2MqttUnity;

/// <summary>
/// Examples for the M2MQTT library (https://github.com/eclipse/paho.mqtt.m2mqtt),
/// </summary>
namespace M2MqttUnity.Examples
{
    /// <summary>
    /// Script for testing M2MQTT with a Unity UI
    /// </summary>
    public class MQTT_PubSub : M2MqttUnityClient
    {

        public InputField input_ssid, input_pw, input_brokerSettings;



        [Tooltip("Set this to true to perform a testing cycle automatically on startup")]
        bool autoTest = false;
        [Header("User Interface")]
        InputField consoleInputField;
        Toggle encryptedToggle;
        InputField portInputField;

        private bool updateUI = false;

        ArrayList receiveMessageQueue; // storing MQTT messages so that they can be attached to UI elements
        ArrayList receiveMessageQueue2;

        public Text text_deviceId;
        public Text text_setIdMsg;


        public void TestPublish()
        {
            client.Publish("hat/msg", System.Text.Encoding.UTF8.GetBytes("Test message"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
            Debug.Log("Test message published");
        }

        public void SetEncrypted(bool isEncrypted)
        {
            this.isEncrypted = isEncrypted;
        }


        public void SetUiMessage(string msg)
        {
            if (consoleInputField != null)
            {
                consoleInputField.text = msg;
                updateUI = true;
            }
        }

        public void AddUiMessage(string msg)
        {
            if (consoleInputField != null)
            {
                consoleInputField.text += msg + "\n";
                updateUI = true;
            }
        }

        protected override void OnConnecting()
        {
            base.OnConnecting();
            SetUiMessage("Connecting to broker on " + brokerAddress + ":" + brokerPort.ToString() + "...\n");
        }

        protected override void OnConnected()
        {
            base.OnConnected();
            SetUiMessage("Connected to broker on " + brokerAddress + "\n");

            if (autoTest)
            {
                TestPublish();
            }
        }

        protected override void SubscribeTopics()
        {
            client.Subscribe(new string[] { "hat/msg" }, new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
            client.Subscribe(new string[] { "hat/getId" }, new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });

        }

        protected override void UnsubscribeTopics()
        {
            client.Unsubscribe(new string[] { "hat/msg" });
            client.Unsubscribe(new string[] { "hat/getId" });
        }

        protected override void OnConnectionFailed(string errorMessage)
        {
            AddUiMessage("CONNECTION FAILED! " + errorMessage);
        }

        protected override void OnDisconnected()
        {
            AddUiMessage("Disconnected.");
        }

        protected override void OnConnectionLost()
        {
            AddUiMessage("CONNECTION LOST!");
        }


        protected override void Start()
        {
            base.Start();

            receiveMessageQueue = new ArrayList(); // storing MQTT messages so that they can be attached to UI elements
            receiveMessageQueue2 = new ArrayList(); // storing MQTT messages so that they can be attached to UI elements

            // brokerAddress = (addressInputField.text == "") ? addressInputField.placeholder.GetComponent<Text>().text : addressInputField.text;
            // brokerAddress = "huhu";
            brokerPort = 1883;
        }

        protected override void DecodeMessage(string topic, byte[] message)
        {
            string msg = System.Text.Encoding.UTF8.GetString(message);

            if (topic.Contains("getId"))
            {
                receiveMessageQueue.Add(msg);
            }

            if (topic.Contains("msg"))
            {
                receiveMessageQueue2.Add(msg);
            }



            if (topic == "M2MQTT_Unity/test")
            {
                if (autoTest)
                {
                    autoTest = false;
                    Disconnect();
                }
            }
        }


        private void ProcessMessage(string msg)
        {
            AddUiMessage("Received: " + msg);
        }

        protected override void Update()
        {
            base.Update(); // call ProcessMqttEvents()

            if (receiveMessageQueue.Count >= 1)
            {
                print("receiveMessageQueue[0]: " + receiveMessageQueue[0]);
                string queue_input = (string)receiveMessageQueue[0];
                receiveMessageQueue.RemoveRange(0, 1);
                StartCoroutine(GetIdMessage(queue_input));
            }

            if (receiveMessageQueue2.Count >= 1)
            {
                string queue_input = (string)receiveMessageQueue2[0];
                receiveMessageQueue2.RemoveRange(0, 1);
                StartCoroutine(SetIdMessage(queue_input));
            }
        }

        private void OnDestroy()
        {
            Disconnect();
        }

        private void OnValidate()
        {
            if (autoTest)
            {
                autoConnect = true;
            }
        }










        public void OnAnim_1Hat_black(InputField input_deviceId)
        {
            string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            print("deviceId: " + deviceId);
            client.Publish("hat/" + deviceId, System.Text.Encoding.UTF8.GetBytes("a1"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);
        }

        // WHITE constant solid (called via buttons)
        public void OnAnim_1Hat_white(InputField input_deviceId)
        {
            string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            print("deviceId: " + deviceId);
            client.Publish("hat/" + deviceId, System.Text.Encoding.UTF8.GetBytes("a2"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);

        }

        // Running Light WHITE (called via buttons)
        public void OnAnim_1Hat_runningWhite(InputField input_deviceId)
        {
            string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            print("deviceId: " + deviceId);
            client.Publish("hat/" + deviceId, System.Text.Encoding.UTF8.GetBytes("a3"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);
        }

        // Pulsing Light WHITE (called via buttons)
        public void OnAnim_1Hat_pulsingWhite(InputField input_deviceId)
        {
            string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            print("deviceId: " + deviceId);
            client.Publish("hat/" + deviceId, System.Text.Encoding.UTF8.GetBytes("a4"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);
        }

        // ORANGE constant solid (called via buttons)
        public void OnAnim_1Hat_orange(InputField input_deviceId)
        {
            string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            print("deviceId: " + deviceId);
            client.Publish("hat/" + deviceId, System.Text.Encoding.UTF8.GetBytes("a5"), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);
        }

        /* 
         * When the anim button "ALL HATS" is pressed
         */
        public void OnAnim_AllHats(string msg)
        {
            // MQTTBehaviour.Publish("hat/all", msg);
            client.Publish("hat/all", System.Text.Encoding.UTF8.GetBytes(msg), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);

        }

        // when the button Set ID/"ASSIGN" is pressed
        public void OnSetDeviceId(InputField input_deviceId)
        {
            string newDeviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
            // print(mqttAddress + "   |   " + newDeviceId);
            client.Publish("hat/setId", System.Text.Encoding.UTF8.GetBytes("i" + newDeviceId), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);

            // print("hat/setId: "+ newDeviceId);
        }

        // when the button Set ID/"Set SSID, PW, Broker IP" is pressed
        public void OnSetupBroker()
        {
            string ssid = (input_ssid.text == "") ? input_ssid.placeholder.GetComponent<Text>().text : input_ssid.text;
            string pw = (input_pw.text == "") ? input_pw.placeholder.GetComponent<Text>().text : input_pw.text;
            string brokerSettings = (input_brokerSettings.text == "") ? input_brokerSettings.placeholder.GetComponent<Text>().text : input_brokerSettings.text;

            client.Publish("hat/ssid", System.Text.Encoding.UTF8.GetBytes(ssid), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
            client.Publish("hat/pw", System.Text.Encoding.UTF8.GetBytes(pw), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
            client.Publish("hat/broker", System.Text.Encoding.UTF8.GetBytes(brokerSettings), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
            // print(mqttAddress + "   |   " + ssid + "   |   " + pw + "   |   " + brokerSettings);
        }

        IEnumerator GetIdMessage(string receivedText)
        {
            text_deviceId.text = receivedText;
            yield return new WaitForSeconds(6);
            text_deviceId.text = "-";
            yield return null;
        }

        IEnumerator SetIdMessage(string receivedText)
        {
            text_setIdMsg.text = receivedText;
            yield return new WaitForSeconds(3);
            text_setIdMsg.text = "";
            yield return null;
        }

    }
}
