
Troy Giunipero
17 May 2010
Joseph Caprino
3 May 2016 (Edits)

For detailed setup instructions corresponding to the ORIGINAL Affablebean Project, see:  
http://netbeans.org/kb/docs/javaee/ecommerce/setup.html (note that our project is similar, but has 
some major differences such as database schemas)

The following is a coarse-grained summary of setup steps and requirements.

The AffableBean application requires access to a MySQL database.

Before running the application,

 1. Download MySQL from: http://dev.mysql.com/downloads/

    The database server configuration uses 'root' / '' as the default username / password.
    Because GlassFish sometimes doesn't accept an empty password when creating a connection
    pool*, the AffableBean project requires that you set the server password to: 'nbuser'.

    You can set the password from a command-line prompt. To set your password to 'nbuser',
    navigate to your MySQL installation's 'bin' directory in the command-line prompt and
    enter the following:

        shell> mysql -u root
        mysql> UPDATE mysql.user SET Password = PASSWORD('nbuser') WHERE User = 'root';
        mysql> FLUSH PRIVILEGES;

    *See GlassFish Issue 12221:  https://glassfish.dev.java.net/issues/show_bug.cgi?id=12221

 2. From the IDE's Services window, right-click the MySQL Server node and choose Create Database.

 3. In the Create Database dialog, type in 'affablebean' and select the 'Grant Full Access
    To *@localhost' option.

 4. Click OK to exit the dialog.

 5. Run the SQL scripts found in this directory. The schemaCreation script creates tables
    necessary for the application. The sampleData script adds sample data to the
    tables. The ratingSchema builds the rating database schema. The ratingSchema_PMF_Test_Schema
    builds a test schema for the PMF Recommender System and populates some values to prep for 
    testing with the data discussed in Appendix A at the bottom of this document. The ratingSchema_PMF_Loading_Sample_Training_Data part of the testing data discussed in Appendix A. 
    Run the schemaCreation script first, then run the sampleData script, then run the
    ratingSchema script. If you would like test data for the PMF recommender system, then run 
    the ratingSchema_PMF_Test_Schema, and then see Appendix A. To run scripts:

    a. Double-click each script node to open them in the IDE's editor.
    b. In the toolbar above the editor, make sure the connection to the
       'affablebean' database is selected:

        jdbc:mysql://localhost:3306/affablebean

    c. Click the Run SQL button to run the script.

 6. It is necessary to enable automatic driver deployment on the server. This is because the
    GlassFish server doesn't contain the MySQL driver. Choose Tools > Servers, and select the
    GlassFish server in the left pane. Then in the right pane, select the 'Enable JDBC Driver
    Deployment' option. (The option is enabled by default.)

    After making this change, you'll need to restart the server (if it's already running).

 7. To make the admin features functional, you will need to 


    a. edit the Glassfish server so that
    the admin user has the proper permissions to access the pages she needs to access. In the 
    NetBeans IDE, navigate to the Services tab -> Servers -> GlassFish Server -> right-click 
    and select "View Domain Admin Console" -> Configurations -> Server Config -> Security ->
    Realms -> File -> Manage Users -> New. Follow the instructions here, specifically Step 8:
    https://netbeans.org/kb/docs/javaee/ecommerce/security.html at the section titled "Create
    Users and/or Groups on the Server". 
    b. Additionally, with GlassFish Server 4, also navigate to "View Domain Admin Console" -> 
    Configurations -> Default Config -> Security and check the box "Default Principal to Role Mapping"
    and then click save. Do the same at "View Domain Admin Console" -> 
    Configurations -> Server Config -> Security .
    c. Finally, navigate to ~/glassfish-[version number]/glassfish/ (Windows: 
    C:\Program Files\glassfish-[version]\) and edit glassfish\domains\domain1\config\domain.xml
    so that <network-listener is on ports 9090 instead of 8080 and 9191 instead of 8181.
    f. Restart the IDE and re-run the website.


Notes:

    - The sun-resources.xml file creates the 'jdbc/affablebean' data source, and 'AffablebeanPool'
      connection pool on the server when the application is deployed.

    - The server may need to be restarted for the data source and connection pool settings to take
      effect.

    - The application uses EclipseLink as the persistence provider, and was developed using NetBeans
      6.8 and 6.9 with GlassFish 3.

Netbeans 8 Users:
    - If using Netbeans 8 with its accompanying Glassfish Server 4, you may need to copy the mysql connector
      jar from this location: in the NetBeans IDE, navigate to the Services tab on the top left. Click on 
      Services -> Drivers -> MySQL (Connector/J driver). Right-click on it and click "Customize". Using
      the location listed here, copy the mysql-connector-java jar to inside your GlassFish 
      installation directory: ~/glassfish-[version number]/glassfish/lib (Windows: C:\Program Files\glassfish-[version]).
      Restart your IDE after this.
    

APPENDIX A: Sample Data for Testing the Recommender System:
Note that, in this folder, there is a file called “ratingSchema_PMF_Loading_Sample_Training_Data.sql”. This file can be used to import sample data for the PMF recommender system from the following webpage: 
http://www.cs.toronto.edu/~rsalakhu/BPMF.html

Download the http://www.cs.toronto.edu/~rsalakhu/code_BPMF/moviedata.mat datafile. 
Using either Matlab or Octave, you can import this data, and then re-export its arrays to separate csv files per the aforementioned sql file. 
Make sure the columns are proper: 
 The training data user_id value can reach up to 6040
 The training data movie_id value can reach up to 3952
 The training data rating can be an int from 1 to 5
Use this information to match columns properly.

    
