import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class ClientVigenere {
    public static void main(String[] args) {
        try {
            // Get reference to the RMI registry on the localhost and port 10199
            Registry registry = LocateRegistry.getRegistry("localhost", 10199);

            // Look up the remote server object in the registry
            RemoteServerInterface remoteServer = (RemoteServerInterface) registry.lookup("RemoteServer");

            // Specify the Vigenere cipher keyword
            String keyword = "GORDIAN";  // Replace with the actual Vigenere cipher keyword

            // Decrypt the message using the remote server
            String decryptedMessage = remoteServer.decryptVigenere(keyword);

            // Print the decrypted message
            System.out.println("Decrypted Message (Vigenere Cipher):\n" + decryptedMessage);
            
        } catch (Exception e) {
            // Handle exceptions
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
        }
    }
}
