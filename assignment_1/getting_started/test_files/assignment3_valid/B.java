public class B {
  public static void main(String[] a) {
    System.out.println(new FacFib().Compute(5, 10));
  }
}

class FacFib {
  public int Compute(int first, int second) {
    int fac;
    int fib;
    int returnValue;

    if (!(first < 1) && !(second < 1)) {
      fac = this.ComputeFac(first);
      System.out.println(fac);

      fib = this.ComputeFib(second);
      System.out.println(fib);
      returnValue = 1;
    } else {
      returnValue = 0;
    }
    return returnValue;
  }

  public int ComputeFac(int num) {
    int fac;
    if (num < 1)
      fac = 1;
    else
      fac = num * (this.ComputeFac(num - 1));
    return fac;
  }

  public int ComputeFib(int num) {
    int f0;
    int f1;
    int fib;
    int i;

    f0 = 0;
    f1 = 1;
    fib = num;

    i = 1;
    while (i < num) {
      fib = f1 + f0;
      f0 = f1;
      f1 = fib;
      i = i + 1;
    }
    return fib;
  }

}
