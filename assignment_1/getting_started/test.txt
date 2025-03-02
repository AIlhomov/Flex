public class LittleTest {
    public static void main(String[] a) {
        System.out.println(new MyClass().func());
    }
}

class MyClass {
    Element x;
    int a;
    boolean c;
    boolean d;
    int[] e;

    public int func() {
        int[] newVar;
        x = new element();// @error - semantic ('element' is not a valid Class)
        c = new Element();// @error - semantic ('c' and expression 'Element' new are of different types)

        a = 2 * e[3 * 5];

        newVar[3] = 0;
        a = e.length;
        a = e[1].length;// @error - semantic ('e' is undefined or wrong)
        x = e.length;// @error - semantic ('x' and expression 'e.length' are of different types)

        x = new int[1];// @error - semantic ('x' and expression new int[1] new are of different types)
        newVar = new int[true];// @error - semantic ('true': is of wrong type)
        newVar = new int[1];

        return x.InInt();// @error - semantic (missing parameter)
    }

    public int func2() {
        return x.InInt(1);
    }
}

class Element {
    int a;
    boolean b;
    Element c;

    public int InInt(int x) {
        return a;
    }

    public int InInt2(int x) {
        return x + 1;
    }

    public Element InClass() {
        return c;
    }

    public boolean InBool() {
        return true;
    }

    public boolean InBool2() {
        return b;
    }
}