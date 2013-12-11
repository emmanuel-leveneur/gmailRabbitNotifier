var http = require('http');
var MailListener = require("mail-listener2");
var mailListener = new MailListener({
  username: "leveneur.emmanuel.florent@gmail.com", // gmail mail here
  password: "",  // gmail password here
  host: "imap.gmail.com",
  port: 993,
  mailbox: "INBOX", 
  markSeen: true, 
  fetchUnreadOnStart: true
});
mailListener.start(); // start listening
mailListener.on("server:connected", function(){
  console.log("imapConnected");
});
mailListener.on("error", function(err){
  console.log(err);
});
mailListener.on("mail", function(mail){
  console.log('Un nouveau mail est arrivé');
  http.get("http://10.230.130.65/notify.cgi", function(res){
 	  console.log("Request response: " + res.statusCode);
  });
});
