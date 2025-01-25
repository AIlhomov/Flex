public class OperatorPriority {
    public static void main(String[] a) {
        System.out.println(new OP().arithmetic());
    }
}

class OP {

    public int arithmetic() {
        int b;
        b = 4;
        int c;
        c = 8;
        int ret;
        // the following statements should generate the same tree
        ret = 1 + 18 * 3 - b + c * 9;
        ret = 1 + (18 * 3) - b + (c * 9);
        return ret;
    }

    public boolean relational() {
        int a;
        int b;
        boolean c;
        boolean d;
        a = 10;
        b = 12;
        d = false;
        c = true;
        boolean ret;
        // the following statements should generate the same tree
        ret = a < b && !d || d == !c;
        ret = ((a < b) && (!d)) || (d == (!c));
        return ret;
    }
}
