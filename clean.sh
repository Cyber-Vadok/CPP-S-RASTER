# Prompt the user for confirmation
read -p "Are you sure you want to delete the directories (plots, input, parameter_output, time_output, data_output)? [y/N] " confirm

# Check the user's response
if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
    echo "Proceeding with cleanup..."
    rm -r plots
    rm -r input
    rm -r parameter_output
    rm -r time_output
    rm -r data_output
    echo "Cleanup completed."
else
    echo "Cleanup aborted."
fi