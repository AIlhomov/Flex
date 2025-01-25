public class ExtraSemicolon {
    public static void main(String[] args) {
        number = 10;
        ; // @error - syntax (extra semi colon)
        System.out.println(number);
    }
}
