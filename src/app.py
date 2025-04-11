from nbformat import v4, write

with open("saida.txt", "r") as f:
    output = f.read()


notebook = v4.new_notebook()

code_cell = v4.new_code_cell("")

code_cell.outputs = [
    v4.new_output("stream", name="stdout", text=output)
]

notebook.cells.append(code_cell)

with open("estatisticas.ipynb", "w") as f:
    write(notebook, f)

print("Notebook gerado como saida_cpp.ipynb")
