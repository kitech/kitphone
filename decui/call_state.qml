import Qt 4.7


 Rectangle {
    id: top_win;
     width: 300; height: 120;
     border.width: 10; border.color: "#101C32";
     radius: 3;
     Row {
        id: row1;
         width: top_win.width; height: 30; x: 2; y: 2;
         spacing: 3;
         Row {}
         Image {
            x: 0;
            source: "../skins/default/speaker.png"            
         }
         Text {
            width: top_win.width - 16 * 2 - 4 - 2; 
            text: "call status here...";
        }
        Image {
            x: top_win.width - 16 - 2;
            smooth: true;
            source: "../skins/default/Un-mute_Alpha_down_16x16_s1.png";
        }

    }

    
    Row {
        id: row2;
        x: 2; y: 32; width: top_win.width; height: 60;
        spacing: 3;
        Column {
            width: 30; height: row2.height;
            Rectangle {
                width: 30; height: row2.height;
                color: "red";
            }
        }
        Image {
            x: 6; y: 3; width: row2.height-8; height: row2.height-8;
            source: "../skins/default/user.png";
        }
        Column {
            spacing: 5;
            Text {
                x: +22; y: 3; height: row2/2;
                text: "phone678990873.......";
            }
            Text {
                x: +22; y: 30; height: row2/2;
                text: "phone678990873.......";
            }
        }
        Image {

        }
        Column {
            id: mic_vol_win;
            x: top_win.width-mic_vol_win.width; width: 30; height: row2.height;
            Rectangle {
                width: 30; height: row2.height;
                color: "red";
            }
        }
    }

    Row {
        id: row3;
        x: 2; y: top_win.height - 30 + 3; width: top_win.width; height: 30;
        Image {
            width: 22; height: 22;
            source: "../skins/default/Call_video_start_GreenStart_32x32_s1.png";
        }

        Image {
            x: row3.width-25; width: 22; height: 22;
            source: "../skins/default/hangup.png";
        }
    }

     gradient: Gradient {
         GradientStop { position: 0.0; color: "red" }
         GradientStop { position: 0.33; color: "yellow" }
         GradientStop { position: 1.0; color: "green" }
     }
 }