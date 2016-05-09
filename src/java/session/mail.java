package session;

import java.io.*;
import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

public class mail 
{ 
    public static void sendActivation(String to, String sub, 
                         String msg, final String user,final String pass)
    { 
        String to1=to;
        String sub1=sub;
        String msg1=msg;
     //create an instance of Properties Class   
     Properties props = new Properties();
     
     /* Specifies the IP address of your default mail server
     	   for e.g if you are using gmail server as an email sever
           you will pass smtp.gmail.com as value of mail.smtp host. 
           As shown here in the code. 
           Change accordingly, if your email id is not a gmail id
        */
     props.put("mail.smtp.host", "smtp.gmail.com");
     //below mentioned mail.smtp.port is optional
     props.put("mail.smtp.port", "587");		
     props.put("mail.smtp.auth", "true");
     props.put("mail.smtp.starttls.enable", "true");
     
     
     /* Pass Properties object(props) and Authenticator object   
           for authentication to Session instance 
        */

    Session session = Session.getInstance(props,new javax.mail.Authenticator()
    {
  	  protected PasswordAuthentication getPasswordAuthentication() 
  	  {
  	 	 return new PasswordAuthentication("researchparticipations@gmail.com","uncc@2015");
  	  }
   });
    
   try
   {
 
 	/* Create an instance of MimeMessage, 
 	      it accept MIME types and headers 
 	   */
 
    MimeMessage message = new MimeMessage(session);
       message.setFrom(new InternetAddress("researchparticipations@gmail.com"));
       message.addRecipient(Message.RecipientType.TO,new InternetAddress(to1));
       message.setSubject(sub1);
       message.setText(msg);

       /* Transport class is used to deliver the message to the recipients */
       
       Transport.send(message);
 
 
    }
    catch(Exception e)
    {
    	 e.printStackTrace();
    }
  }
    
     
}