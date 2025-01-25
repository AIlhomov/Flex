public class DanglingElse {
    public static void main(String[] a) {
        System.out.println(new AST().TestIfElse());
    }
}

class AST {

    public int TestIfElse() {
        boolean A;
        boolean B;
        A = false;
        B = true;
        // The following if-statements should generate the same tree.
        if (A)
            if (B)
                B = true;
            else
                B = false;

        if (A) {
            if (B)
                B = true;
            else
                A = false;
        }

        return 1;
    }

}
