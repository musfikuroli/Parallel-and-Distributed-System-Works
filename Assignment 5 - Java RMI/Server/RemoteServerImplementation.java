import java.io.BufferedReader;
import java.io.FileReader;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.io.IOException;

public class RemoteServerImplementation extends UnicastRemoteObject implements RemoteServerInterface {
    // private static final long serialVersionUID = 1L;
    private static final String ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    public RemoteServerImplementation() throws RemoteException {
        // Constructor must throw RemoteException
    }

    // Function for Reading Cipher Text from ""Cipher.txt"
    private String readCipherText() throws RemoteException{
        try (BufferedReader bufferedReader = new BufferedReader(new FileReader("Cipher.txt"))) {
            StringBuilder stringBuilder = new StringBuilder();
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                stringBuilder.append(line);
            }
            return stringBuilder.toString();
        } catch (Exception e) {
            throw new RemoteException("Error reading cipher text file.", e);
        }
    }

    // Function for Reading Tabula Recta from "TabulaRecta.txt"
    private char[][] readTabulaRecta() throws IOException {
        try (BufferedReader bufferdReader = new BufferedReader(new FileReader("TabulaRecta.txt"))) {
            char[][] tabulaRecta;
            tabulaRecta = new char[ALPHABET.length()][ALPHABET.length()];
            String line;

            for (int i = 0; i < ALPHABET.length(); i++) {
                line = bufferdReader.readLine();
                tabulaRecta[i] = line.toCharArray();
            }

            return tabulaRecta;
        }
    }

    // Function for Finding Row Index
    private int findIndexInRow(char[] row, char target) {
        for (int i = 0; i < row.length; i++) {
            if (row[i] == target) {
                return i;
            }
        }
        return -1; // not found
    }

    @Override
    public String decryptCaesar(String keyword) throws RemoteException {
        try {
            // Read the cipher text from the file
            String encryptedText = readCipherText();
    
            // Ensure the keyword is in uppercase for consistency
            keyword = keyword.toUpperCase();
    
            // StringBuilder to store the decrypted text
            StringBuilder decryptedText = new StringBuilder();
    
            // Convert the encrypted text to a char array for iteration
            char[] encryptedChars = encryptedText.toCharArray();
    
            // Iterate through each character in the encrypted text
            for (int i = 0; i < encryptedChars.length; i++) {
                char c = encryptedChars[i];
    
                // Check if the character is a letter
                if (Character.isLetter(c)) {

                    // Calculate the Shift of The Respective Character of The Key from 'A'
                    int shift = keyword.charAt(i % keyword.length()) - 'A';

                    // Find the row index of the Current Character of The Key
                    int indexOfCurrentEncryptedLetter = ALPHABET.indexOf(c);
    
                    // Decrypt the letter using Caesar cipher
                    // char decryptedChar = (char) ((c - shift - 'A' + 26) % 26 + 'A');
                    char decryptedChar = ALPHABET.charAt((indexOfCurrentEncryptedLetter - shift + 26) % 26);
    
                    // Append the decrypted character to the result
                    decryptedText.append(decryptedChar);
                } else {
                    // Non-letter characters are appended as is
                    decryptedText.append(c);
                }
            }
    
            // Convert the StringBuilder to a String and return the result
            return decryptedText.toString();
    
        } catch (IOException e) {
            // Handle IO exception (file reading error)
            throw new RemoteException("Error decrypting with Caesar cipher. Failed to load Cipher text.", e);
        }
    }
    


    @Override
    public String decryptVigenere(String keyword) throws RemoteException {
    try {
        // Read the cipher text from the file
        String encryptedText = readCipherText();

        // Read the Tabula Recta from the file
        char[][] tabulaRecta = readTabulaRecta();

        // Ensure the keyword is in uppercase for consistency
        keyword = keyword.toUpperCase();

        // StringBuilder to store the decrypted text
        StringBuilder decryptedText = new StringBuilder();

        // Length of the keyword
        int keyLength = keyword.length();

        // Loop through each character in the encrypted text
        for (int i = 0; i < encryptedText.length(); i++) {
            // Current character in the encrypted text
            char c = encryptedText.charAt(i);

            // Current character in the keyword (repeated if necessary)
            char keyChar = keyword.charAt(i % keyLength);

            // Check if the current character is an uppercase letter
            if (Character.isUpperCase(c)) {
                // Find the row index of the Current Character of The Key
                int rowIndex = ALPHABET.indexOf(keyChar);

                // Find the column index corresponding to the current encrypted character in the Tabula Recta
                int colIndex = findIndexInRow(tabulaRecta[rowIndex], c);

                // Get the decrypted character from the first row of the alphabet
                char decryptedChar = ALPHABET.charAt(colIndex);

                // Append the decrypted character to the StringBuilder
                decryptedText.append(decryptedChar);
            }
            // Check if the current character is a lowercase letter
            else if (Character.isLowerCase(c)) {
                // Find the row index corresponding to the uppercase version of the current keyword character
                int rowIndex = ALPHABET.indexOf(Character.toUpperCase(keyChar));

                // Find the column index corresponding to the uppercase version of the current encrypted character in the Tabula Recta
                int colIndex = findIndexInRow(tabulaRecta[rowIndex], Character.toUpperCase(c));

                // Get the decrypted character from the first row of the alphabet and convert it to lowercase
                char decryptedChar = ALPHABET.charAt(colIndex);

                // Append the decrypted character to the StringBuilder
                decryptedText.append(Character.toLowerCase(decryptedChar));
            }
            // If the current character is not a letter, append it as is
            else {
                decryptedText.append(c);
            }
        }

        // Return the final decrypted text as a string
        return decryptedText.toString();
        } catch (IOException e) {
            // Handle IO exception (e.g., file not found)
            throw new RemoteException("Error decrypting with Vigenere cipher. Failed to load TabulaRecta.", e);
    } catch (Exception e) {
        // Handle other exceptions
        throw new RemoteException("Error decrypting with Vigenere cipher.", e);
    }
    }
}