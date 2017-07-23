var Thread = Java.type("java.lang.Thread");
var ACATopic = "home/prova"

function execute(action) {
  out("Test Script: " + action.getName());
  for (var i = 0; i < 500; i++) {
    for (var id = 1; id < 5; id++) {

      // if (i%2 == 0) {
      //   x = 25;
      //   y = 45;
      //   z = 65;
      // }
      // else {
      //   x = 35;
      //   y = 55;
      //   z = 75;
      // }
      x = Math.random()*100;
      y = Math.random()*100;
      z = Math.random()*100;
      t = i;
      // if (id == 1) {
      //   x = 10 + Math.random()*10;
      //   y = 20 + Math.random()*10;
      //   z = 30 + Math.random()*10;
      // }
      // else if (id == 2) {
      //   x = 40 + Math.random()*10;
      //   y = 50 + Math.random()*10;
      //   z = 60 + Math.random()*10;
      // }
      // else {
      //   x = Math.random()*100;
      //   y = Math.random()*100;
      //   z = Math.random()*100;
      // }
      out("\nID = " + id + "\nT = " + t + "\nX = " + x + " - Y = " + y + " - Z = " + z);
      mqttManager.publish(ACATopic, id + ";" + t + ";" + x + ";" + y + ";" + z);
    }
    Thread.sleep(1000);
  }
  action.setExitCode(0);
  action.setResultText("done.");
  out("Test Script: Done");
  return action;
}

function out(message){
  output.print(message);
}
