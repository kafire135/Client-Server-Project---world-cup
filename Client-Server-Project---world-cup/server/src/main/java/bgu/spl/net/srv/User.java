package bgu.spl.net.srv;

public class User {

    private String name;
    private String password;
    private boolean isConnected;

    public User(String name, String password)
    {
        this.name = name;
        this.password = password;
        isConnected = false;
    }

    public String getName(){
        return name;
    }

    public String getPassword(){
        return password;
    }

    public boolean isConnected(){
        return isConnected;
    }

    public void connect(){
        isConnected = true;
    }

    public void dissconnect(){
        isConnected = false;
    }

    public boolean checkPassword(String password){
        return (this.password.equals(password));
    }
    
}
