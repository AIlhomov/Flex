public class D1 {
  public static void main(String[] a) {
    System.out.println(new Sum().calcSum(100));
  }
}

class Sum {
  public int calcSum(int num) {
    int sum;
    sum = 0;
    while (0 < num) {
      sum = sum + num;
      num = num - 1;
    }
    return sum;
  }
}
