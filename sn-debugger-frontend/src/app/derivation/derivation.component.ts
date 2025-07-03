import { Component, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';

@Component({
  selector: 'app-derivation',
  templateUrl: './derivation.component.html',
  standalone: false
})
export class DerivationComponent {
  derivationhtml: string = '';
  countlogentries: number = 0;
  threadnum: number = 0;           // You may want to @Input this later
  maxderivation: number = 30;

  @ViewChild('derivationsid') derivationsEl!: ElementRef<HTMLDetailsElement>;

  constructor(private http: HttpClient, private sanitizer: DomSanitizer) {}

  loadderivations(opening: boolean): void {
    const field = this.derivationsEl?.nativeElement;

    if ((opening && !field?.open) || (!opening && field?.open)) {
      const url = `http://127.0.0.1/derivation.json?threadnum=${this.threadnum}&maxlogentries=${this.maxderivation}`;
      this.http.get<any>(url).subscribe(response => {
        this.derivationhtml = response.derivationhtml;
        this.countlogentries = response.countlogentries ?? 0;
      });
    }
  }

  trustAsHtml(html: string): SafeHtml {
    return this.sanitizer.bypassSecurityTrustHtml(html);
  }
}
