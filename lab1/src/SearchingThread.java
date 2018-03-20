import javax.swing.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.*;
import java.util.ArrayList;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author TimoshenkoStas
 */
public class SearchingThread implements Runnable {
    
    private static String defaultFileExtensions = "{*.txt}";
    private static Object lock = new Object();
    private String directoryName;
    private boolean withSubdir;
    private String searchLine;
    private PathMatcher matcher;
    private ArrayList<String> results;
    private boolean stopped;
    private boolean pause;
    private Thread thread;
    private JTextArea textArea;


    SearchingThread(String directoryName, String template, String searchLine,
                     boolean withSubdir, JTextArea textArea) {

        setSearchOptions(directoryName, template, searchLine, withSubdir);

        this.thread = new Thread(this);
        this.stopped = false;
        this.pause = false;
        this.textArea = textArea;
        this.results = new ArrayList<>();
    }

    private void setSearchOptions(String directoryName, String template, String lineForSearch, boolean withSubdirectories) {
        if (template == null) {
            this.matcher = (lineForSearch == null) ? null : FileSystems.getDefault().getPathMatcher(
                    "glob:" + defaultFileExtensions);
        } else {
            this.matcher = FileSystems.getDefault().getPathMatcher("glob:" + template);
        }
        this.directoryName = directoryName;
        this.searchLine = lineForSearch == null ? null : lineForSearch.toLowerCase();
        this.withSubdir = withSubdirectories;
    }
    
    String getDirectoryName() {
        return directoryName;
    }
    
    private void search() {
        if(findInDirectory(directoryName)) {
            textArea.append("***Search completed***\n");
        } else {
            textArea.append("***Search stopped***\n");
        }
        thread.interrupt();
    }


    private boolean findInDirectory(String path) {
        File root = new File(path);
        File[] list = root.listFiles();
        if (list == null) {
            return true;
        }

        for (File file : list) {
            if (file.isFile()) {
                if (check(file.toPath())) {
                    textArea.append(textArea.getLineCount() + ". " + file.getAbsolutePath() + "\n");
                    results.add(file.getAbsolutePath());
                }
            } else if (withSubdir) {
                findInDirectory(file.getAbsolutePath());
            }

            pauseCheck();
            if (stopped) {
                return false;
            }
        }
        return true;
    }

    private boolean check(Path filePath) {
        if (matcher != null && searchLine != null) {
            return matcher.matches(filePath.getFileName()) && containsLine(filePath.toString());
        }
        if (matcher != null) {
            return matcher.matches(filePath.getFileName());
        }
        return true;
    }

    private synchronized boolean containsLine(String path) {
        synchronized (lock) {
            try (BufferedReader reader = new BufferedReader(new FileReader(path))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.toLowerCase().contains(searchLine)) {
                        return true;
                    }
                }
            } catch (IOException e) {
                System.err.println(e.getMessage());
                textArea.append("Access denied\n");
            }
        }
        return false;
    }

    @Override
    public void run() {
        search();
    }

    void toStart() {
        if (!pause) {            
            stopped = false;
            textArea.setText("");
            results.clear();    
            thread.start();
        } else {                
            pause = false;
        }
    }

    void toPause() {
        pause = true;
    }
    
     private void pauseCheck() {
        while (pause) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
                textArea.append(e.getMessage());
            }
        }
    }

    void toStop() {
        stopped = true;
        pause = false;
    }
    
    public boolean isStop(){
        return stopped;
    }

}
