import matplotlib.pyplot as plt
import os
from sklearn.metrics import classification_report

def plot_roc_curve(fpr, tpr, roc_auc, dataset_type, output_folder):
    """Plots and saves the ROC curve."""
    plt.figure()
    plt.plot(fpr, tpr, color='darkorange', lw=2, label=f'ROC curve (area = {roc_auc:.2f})')
    plt.plot([0, 1], [0, 1], color='navy', lw=2, linestyle='--')
    plt.xlim([0.0, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title(f'Receiver Operating Characteristic - {dataset_type}')
    plt.legend(loc="lower right")
    
    # Save the ROC curve
    roc_path = os.path.join(output_folder, f"roc_curve_{dataset_type}.png")
    plt.savefig(roc_path)
    plt.close()
    
def plot_combined_roc_curve(all_fpr, all_tpr, all_auc, k, output_folder):
    """Plots and saves the combined ROC curve for all folds."""
    plt.figure()
    for i in range(k):
        plt.plot(all_fpr[i], all_tpr[i], lw=2, label=f'Fold {i+1} (AUC = {all_auc[i]:.2f})')
    plt.plot([0, 1], [0, 1], color='navy', lw=2, linestyle='--')
    plt.xlim([0.0, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title(f'Combined ROC Curve for {k}-Fold Cross Validation')
    plt.legend(loc="lower right")
    plt.savefig(os.path.join(output_folder, f'combined_roc_curve_{k}_fold.png'))
    plt.close()
    
    
def plot_threshold_metrics(thresholds, metrics, metric_name, data_type, output_folder):
    plt.figure(figsize=(10, 6))
    plt.plot(thresholds, metrics, color='blue', marker='o')
    plt.xlabel('Threshold')
    plt.ylabel(metric_name)
    plt.title(f'{metric_name} vs. Threshold ({data_type})')
    plt.grid(True)
    plt.savefig(os.path.join(output_folder, f'{metric_name}_vs_threshold_{data_type}.png'))
    plt.close()
    
def save_classification_reports_for_thresholds(thresholds, y_true, y_prob, output_folder, prefix):
    """Generate and save classification reports for different thresholds in a single file."""
    report_file_path = os.path.join(output_folder, f'{prefix}_report_thresholds.txt')
    
    with open(report_file_path, "w") as f:
        for threshold in thresholds:
            y_pred = (y_prob >= threshold).astype(int)
            report = classification_report(y_true, y_pred)
            f.write(f"Classification Report for Threshold {threshold:.2f}:\n")
            f.write(report)
            f.write("\n" + "="*50 + "\n")  # Add a separator for readability
