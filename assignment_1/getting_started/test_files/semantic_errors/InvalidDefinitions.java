public class InvalidDefinitions {
    public static void main(String[] a) {
        System.out.println(new MyClass().aFunc());
    }
}

class MyClass {
    int a;
    boolean b;
    int[] c;
    classdata d;

    public int aFunc() {
        e = a; // @error - semantic ('e' does not exist in the current scope)
        a = d.yFunc();// @error - semantic ('a' and expression 'd.yFunc()' are of different types)
        a = d.zFunc();// @error - semantic ('a' and expression 'd.zFunc()' are of different types)
        return a;
    }

    public boolean bFunc() {
        b = d.zzFunc(); // @error - semantic ('zzFunc' does not exist)
        a = d.xFunc();
        b = this.zFunc(); // @error - semantic ('zFunc' does not exist)
        b = this.aFunc(); // @error - semantic (type mismatch)
        b = d.xFunc();// @error - semantic ('b' and expression 'd.xFunc()' are of different types)
        b = d.zFunc();// @error - semantic ('b' and expression 'd.zFunc()' are of different types)
        return b;
    }

    public int[] cFunc() {
        c = d.xFunc();// @error - semantic ('c' and expression 'd.xFunc()' are of different types)
        c = d.yFunc();// @error - semantic ('c' and expression 'd.yFunc()' are of different types)
        return c;
    }

    public classdata dFunc() {
        d = d.xFunc();// @error - semantic ('d' and expression 'd.xFunc()' are of different types)
        d = d.yFunc();// @error - semantic ('d' and expression 'd.yFunc()' are of different types)
        d = d.zFunc();// @error - semantic ('d' and expression 'd.zFunc()' are of different types)
        d = d;
        return d;
    }

    public boolean EFUNC() {
        a = b;// @error - semantic ('a' and expression 'b' are of different types)
        a = c;// @error - semantic ('a' and expression 'c' are of different types)
        a = d;// @error - semantic ('a' and expression 'd' are of different types)
        b = a;// @error - semantic ('b' and expression 'a' are of different types)
        b = c;// @error - semantic ('b' and expression 'c' are of different types)
        b = d;// @error - semantic ('b' and expression 'd' are of different types)
        c = a;// @error - semantic ('c' and expression 'a' are of different types)
        c = b;// @error - semantic ('c' and expression 'b' are of different types)
        c = d;// @error - semantic ('c' and expression 'd' are of different types)
        return false;
    }

}

class thatdoExist {
    int a;
}

class classdata {
    int x;
    boolean y;
    int[] z;
    classthatdoesntExist pem;// @error - semantic ('classthatdoesntExist' is undefined)
    thatdoExist pen;

    public int xFunc() {
        return x;
    }

    public boolean yFunc() {
        return y;
    }

    public int[] zFunc() {
        return z;
    }
}

class VarUsedBeforeDefinition {
    int a;

    public int b(int b) {
        a = 10; // correct
        b = 10; // correct
        c = 11; // @error - semantic ('c' is not defined yet)
        int c;

        return a;
    }
}