import java.io.File;
import java.io.IOException;
import java.util.Locale;
import java.util.Scanner;

public class EasyTrading {

    public static double mediumValue(double[] a){
        double sum = 0;
        for(int i = 0; i < a.length; i++){
            sum += a[i];
        }
        return sum / a.length;
    }

    public static void main(String[] args) throws IOException{
        Scanner in = new Scanner(new File("input.txt"));
        in.useLocale(Locale.US);
        int m = in.nextInt();
        int n = in.nextInt();
        int k = in.nextInt();
        double[] sharePrice = new double[k];
        for (int i = 0; i < k; i++){
            if(in.hasNextDouble())
                sharePrice[i] = in.nextDouble();
        }
        boolean shareGrowth = false;
        for (int i = n - 1; i < k; i++){
            double[] N = new double[n];
            double[] M = new double[m];
            for(int j = 0; j < n; j++)
                N[j] = sharePrice[i - j];
            for(int j = 0; j < m; j++)
                M[j] = sharePrice[i - j];
            if (mediumValue(N) < mediumValue(M) && !shareGrowth) {
                System.out.println("BUY ON DAY " + (i + 1));
                shareGrowth = true;
            }
            if (mediumValue(N) > mediumValue(M) && shareGrowth) {
                System.out.println("SELL ON DAY " + (i + 1));
                shareGrowth = false;
            }
        }
    }

}
