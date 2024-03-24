import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class RemoteServer {
    public static void main(String[] args) {
        try {
            // Create an instance of the remote object
            RemoteServerImplementation remoteServer = new RemoteServerImplementation();

            // Create and export the RMI registry on port 10199
            Registry registry = LocateRegistry.createRegistry(10199);

            // Bind the remote object's stub in the registry
            registry.rebind("RemoteServer", remoteServer);

            System.out.println("Remote Server Started Successfully.....");

        } catch (Exception e) {
            // Handle exceptions
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }
}
