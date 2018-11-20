import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;

// If you are looking for Java data structures, these are highly useful.
// Remember that an important part of your mark is for doing as much in SQL (not Java) as you can.
// Solutions that use only or mostly Java will not receive a high mark.
// import java.util.Map;
// import java.util.HashMap;
// import java.util.Set;
// import java.util.HashSet;
public class Assignment2 extends JDBCSubmission {

  public Assignment2() throws ClassNotFoundException {

      Class.forName("org.postgresql.Driver");
  }

  @Override
  public boolean connectDB(String url, String username, String password) {
    try {
      this.connection = DriverManager.getConnection(url, username, password);
      String str = "SET SEARCH_PATH TO parlgov";
      this.connection.prepareStatement(str).execute();
      return true;
    } catch (SQLException se) {
      se.printStackTrace();
      return false;
    }
  }

  @Override
  public boolean disconnectDB() {
    try {
      this.connection.close();
      return true;
    } catch (SQLException se) {
      return false;
    }
  }

  @Override
  public ElectionCabinetResult electionSequence(String countryName) {
    try {
      String str1 =
          "select cabinet.id, election_id "
              + "from cabinet join country on cabinet.country_id = country.id "
              + "where country.name = ?";
      PreparedStatement ps = this.connection.prepareStatement(str1);
      ps.setString(1, countryName);
      ArrayList<Integer> elections = new ArrayList<>();
      ArrayList<Integer> cabinets = new ArrayList<>();
      ResultSet resultSet = ps.executeQuery();
      while (resultSet.next()) {
        elections.add(resultSet.getInt(2));
        cabinets.add(resultSet.getInt(1));
      }
      return new ElectionCabinetResult(elections, cabinets);
    } catch (SQLException e) {
      e.printStackTrace();
      return null;
    }
  }

  @Override
  public List<Integer> findSimilarPoliticians(Integer politicianName, Float threshold) {
    try {
      String str1 = "select description from politician_president" + " where id = ?";
      PreparedStatement ps = this.connection.prepareStatement(str1);
      ps.setInt(1, politicianName);
      ResultSet resultSet = ps.executeQuery();
      resultSet.next();
      String polDes = resultSet.getString(1);
      String str2 = "select id,description from politician_president" + "where id != ?";
      ps = this.connection.prepareStatement(str2);
      ps.setInt(1, politicianName);
      resultSet = ps.executeQuery();
      ArrayList<Integer> politician = new ArrayList<>();
      Integer id;
      String des;
      while (resultSet.next()) {
        id = resultSet.getInt(1);
        des = resultSet.getString(2);
	System.out.println(similarity(polDes,des));
        if (similarity(polDes, des) >= threshold && id != politicianName){
          politician.add(id);
        }
      }
      return politician;
    } catch (SQLException e) {
      e.printStackTrace();
      return null;
    }
  }

  public static void main(String[] args) {
    // You can put testing code in here. It will not affect our autotester.
    try{
      Assignment2 ass = new Assignment2();
      String url = "jdbc:postgresql://localhost:5432/csc343h-suntao7";
      ass.connectDB(url,"suntao7","");
      ass.electionSequence("Canada");
      ass.findSimilarPoliticians(9,0);	
    }catch (ClassNotFoundException e) {
      e.printStackTrace();
    }

    //System.out.println("Hello");
  }
}
