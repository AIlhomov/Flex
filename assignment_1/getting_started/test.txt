public class DuplicateIdentifiers {
    public static void main(String[] a) {
        System.out.println(new MyClass().Pen(1, 2));

    }
}

class DuplicateIdentifiers {// @error - semantic (Already Declared Class: 'DuplicateIdentifiers')
    int a;
    int a;// @error - semantic (Already Declared variable: 'a')

    public boolean func() {
        return true;
    }

    public int func() {// @error - semantic (Already Declared Function: 'func')
        return 1;
    }
}

class MyClass {
    boolean b;
    boolean b;// @error - semantic (Already Declared variable: 'b')

    public int Pen(int param, int param) {// @error - semantic (Already Declared parameter: 'param')
        return 1;
    }

    public int Pen2(int param) {
        int param; // @error - semantic (Already Declared parameter: 'param')
        return 1;
    }
}