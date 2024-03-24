import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemoteServerInterface extends Remote {
    
    String decryptCaesar(String keyword) throws RemoteException;

    String decryptVigenere(String keyword) throws RemoteException;
}
