package nand2tetris.parser;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import nand2tetris.parser.HackAssemblyParser.A_commandContext;
import nand2tetris.parser.HackAssemblyParser.C_commandContext;
import nand2tetris.parser.HackAssemblyParser.C_destContext;
import nand2tetris.parser.HackAssemblyParser.C_exprContext;
import nand2tetris.parser.HackAssemblyParser.C_jumpContext;
import nand2tetris.parser.HackAssemblyParser.FileContext;
import nand2tetris.parser.HackAssemblyParser.Label_commandContext;
import nand2tetris.parser.HackAssemblyParser.NameContext;
import nand2tetris.parser.HackAssemblyParser.Name_or_numberContext;
import nand2tetris.parser.HackAssemblyParser.NumberContext;

import org.antlr.v4.runtime.ANTLRFileStream;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.tree.ParseTreeWalker;

import com.google.common.collect.LinkedHashMultimap;
import com.google.common.collect.Multimap;

public class HackAssembler extends HackAssemblyBaseListener {
	private static final SymbolMap C_DEST = new SymbolMap("destination", "c_dest.properties", 2);
	private static final SymbolMap C_EXPR = new SymbolMap("expression", "c_expr.properties", 2);
	private static final SymbolMap C_JUMP = new SymbolMap("jump condition", "c_jump.properties", 2);
	private static final SymbolMap BUILTIN_SYMBOLS = new SymbolMap("symbol", "builtin_symbols.properties");
	
	Map<String, Integer> symbols = new LinkedHashMap<String, Integer>(BUILTIN_SYMBOLS);
	Multimap<String, Integer> symbolReferences = LinkedHashMultimap.create();
	List<Integer> compiled = new ArrayList<Integer>();
	
	public HackAssembler(FileContext file) {
	  new ParseTreeWalker().walk(this, file);
	}
	
	@Override
	public void exitLabel_command(Label_commandContext ctx) {
		NameContext name = ctx.getChild(NameContext.class, 0);
		symbols.put(name.getText(), compiled.size());
	}
	
	@Override
	public void exitA_command(A_commandContext ctx) {
		Name_or_numberContext nameOrNumber = ctx.getChild(Name_or_numberContext.class, 0);
		NameContext name = nameOrNumber.getChild(NameContext.class, 0);
		NumberContext number = nameOrNumber.getChild(NumberContext.class, 0);
		
		if (name != null) {
			symbolReferences.put(name.getText(), compiled.size());
			compiled.add(0); //Symbols will be populated later
		} else {
			compiled.add(Integer.parseInt(number.getText()) & 0x7FFF);
		}
	}

	@Override
	public void exitC_command(C_commandContext ctx) {
		C_destContext dest = ctx.getChild(C_destContext.class, 0);
		C_exprContext expr = ctx.getChild(C_exprContext.class, 0);
		C_jumpContext jump = ctx.getChild(C_jumpContext.class, 0);
		
		int opcode = 0xE000;
		if (jump != null) {
			opcode |= C_JUMP.get(jump.getText()) << 0;
		}
		if (dest != null) {
			opcode |= C_DEST.get(dest.getText()) << 3;
		}
		if (expr != null) {
			opcode |= C_EXPR.get(expr.getText()) << 6;
		}
		compiled.add(opcode);
	}

	@Override
	public void exitFile(FileContext ctx) {
		//After the whole file has been finished,
		//Assign values to unassigned symbols and
		//Populate their values in compiled code
		int nextSymbol = 16;
		for (String symbol : symbolReferences.keys()) {
			if (!symbols.containsKey(symbol)) {
				symbols.put(symbol, nextSymbol++);
			}
			int symbolValue = symbols.get(symbol) & 0x7FFF;
			for (int reference : symbolReferences.get(symbol)) {
				compiled.set(reference, symbolValue);
			}
		}
	}
	
	public static void main(String[] args) throws IOException {
		for (String filename : args) {
			File infile = new File(filename).getCanonicalFile();
			File outfile = new File(infile.getParentFile(), infile.getName().replaceFirst("(\\.([^.]*))?$", ".hack"));
			
			System.out.println("Assembling " + infile + "...");
			CharStream input = new ANTLRFileStream(infile.getPath());
			Lexer lexer = new HackAssemblyLexer(input);
			TokenStream tokens = new CommonTokenStream(lexer);
			HackAssemblyParser parser = new HackAssemblyParser(tokens);
			HackAssembler assembler = new HackAssembler(parser.file());
			
			PrintStream out = new PrintStream(outfile);
		    // Walk it and attach our listener
			for (int cmd : assembler.compiled) {
				String bin = Integer.toBinaryString(cmd);
				while (bin.length() < 16) {
					bin = "0" + bin;
				}
				out.println(bin);
			}
			out.close();
			System.out.println("Finished " + outfile);
		}
	}
}
