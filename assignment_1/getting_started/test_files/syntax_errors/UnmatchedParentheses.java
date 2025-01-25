public class UnmatchedParentheses {
    public static void main(String[] args) {
        result = (10 * (5 + 3); //@error - syntax (Expression with unmatched parentheses)
        System.out.println(result);
    }
}
